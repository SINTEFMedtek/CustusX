/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QStringList>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>

#include <QTime>
#include <QDebug>

// dcmtk includes
#include "dcvrpn.h"

// ctkDICOMCore includes
#include "cxDICOMModel.h"
#include "ctkLogger.h"
#include "boost/shared_ptr.hpp"
#include "ctkDICOMDatabase.h"
#include "cxDicomImageReader.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMModel" );

Q_DECLARE_METATYPE(Qt::CheckState);
Q_DECLARE_METATYPE(QStringList);

namespace cx
{
struct Node;
typedef boost::shared_ptr<Node> NodePtr;

//------------------------------------------------------------------------------
class DICOMModelPrivate
{
  Q_DECLARE_PUBLIC(DICOMModel);
protected:
  DICOMModel* const q_ptr;

public:
  DICOMModelPrivate(DICOMModel&);
  virtual ~DICOMModelPrivate();
  void init();

  QString getFirstDICOMFilename(Node* node) const;
  void fetchChildren(const QModelIndex& indexValue);
  NodePtr createNode(int row, const QModelIndex& parentValue)const;
  Node* nodeFromIndex(const QModelIndex& indexValue)const;
  void fillChildrenUids(NodePtr node) const;
  QString getName(DICOMModel::IndexType type, cx::DicomImageReaderPtr reader) const;
  QVariant getValue(Node* node, int column) const;
  QVariant getUncachedValue(Node* node, int column) const;

  QString getTimestamp(DICOMModel::IndexType type, DicomImageReaderPtr reader) const;
  QString getModality(DICOMModel::IndexType type, DicomImageReaderPtr reader) const;
  int getFrameCountForSeries(QString series) const;
  QString getImageCount(DICOMModel::IndexType type, QString series) const;

  NodePtr RootNode;
  QSharedPointer<ctkDICOMDatabase> DataBase;
  QStringList Headers;
};

//------------------------------------------------------------------------------
// 1 node per row

/** One node representing one of <root/patient/study/series>
  *
  * Children are constructed lazily, using the fetchMore system
  * from QAbstractItemModel.
  */
struct Node
{
  DICOMModel::IndexType Type;
  Node*                           Parent;
  std::vector<NodePtr>            FetchedChildren; ///< all children currently loaded (filled by fetchMore())
  QStringList                     ChildrenUID; ///< uids of all loaded and unloaded children.
  int                             Row;
  QString                         UID;
  std::map<int, QVariant>         CachedValues;

  bool canFetchMore() const
  {
	  return FetchedChildren.size() != ChildrenUID.size();
  }
  bool hasChildren() const
  {
	  return !ChildrenUID.empty();
  }
  NodePtr getFetchedChildForRow(int row) const
  {
	  if (row < this->FetchedChildren.size())
		  return this->FetchedChildren[row];
	  return NodePtr();
  }



};

//------------------------------------------------------------------------------
DICOMModelPrivate::DICOMModelPrivate(DICOMModel& o):q_ptr(&o)
{
}

//------------------------------------------------------------------------------
DICOMModelPrivate::~DICOMModelPrivate()
{
}

//------------------------------------------------------------------------------
void DICOMModelPrivate::init()
{
	Headers.clear();
	Headers << "Name";
	Headers << "Date";
	Headers << "Modality";
	Headers << "Images";
//	Headers << "Info";
}

//------------------------------------------------------------------------------
Node* DICOMModelPrivate::nodeFromIndex(const QModelIndex& indexValue)const
{
	return indexValue.isValid() ? reinterpret_cast<Node*>(indexValue.internalPointer()) : this->RootNode.get();
}
//------------------------------------------------------------------------------
NodePtr DICOMModelPrivate::createNode(int row, const QModelIndex& parentValue)const
{
	NodePtr node(new Node);
	Node* nodeParent = 0;
	if (row == -1)
	{// root node
		node->Type = DICOMModel::RootType;
		node->Parent = 0;
	}
	else
	{
		nodeParent = this->nodeFromIndex(parentValue);
		nodeParent->FetchedChildren.push_back(node);
		node->Parent = nodeParent;
		node->Type = DICOMModel::IndexType(nodeParent->Type + 1);
	}
	node->Row = row;
	if (node->Type != DICOMModel::RootType)
	{
		node->UID = nodeParent->ChildrenUID[row];
	}

	this->fillChildrenUids(node);
	qDebug() << "created node" << node->UID;
	qDebug() << "        node children " << node->ChildrenUID.join(",");

	return node;
}

// add test uid data
void DICOMModelPrivate::fillChildrenUids(NodePtr node) const
{
	if (!DataBase)
		return;

	if (node->Type == DICOMModel::RootType)
	{
		node->ChildrenUID << DataBase->patients();
		node->ChildrenUID << "Patient1" << "Patient2" << "Patient3";
	}
	if (node->Type == DICOMModel::PatientType)
	{
		node->ChildrenUID << DataBase->studiesForPatient(node->UID);
//		node->ChildrenUID << "Study1" << "Study2" << "Study3";
	}
	if (node->Type == DICOMModel::StudyType)
	{
		node->ChildrenUID << DataBase->seriesForStudy(node->UID);
//		node->ChildrenUID << "Series1" << "Series2" << "Series3";
	}
}


QString DICOMModelPrivate::getFirstDICOMFilename(Node* node) const
{
	QStringList series;

	if (node->Type == DICOMModel::RootType)
	{
		qDebug() << "value for root!!!";
		return "";
	}
	else if (node->Type == DICOMModel::PatientType)
	{
		QStringList studies = DataBase->studiesForPatient(node->UID);
		if (studies.empty())
			return "";
		series = DataBase->seriesForStudy(studies[0]);
	}
	else if (node->Type == DICOMModel::StudyType)
	{
		series = DataBase->seriesForStudy(node->UID);
	}
	else if (node->Type == DICOMModel::SeriesType)
	{
		series << node->UID;
	}

	if (series.empty())
		return "";

	QStringList files = DataBase->filesForSeries(series[0]);

	if (files.empty())
		return "";
	return files[0];
}

QVariant DICOMModelPrivate::getValue(Node* node, int column) const
{
	if (!node->CachedValues.count(column))
	{
		QVariant value = this->getUncachedValue(node, column);
		node->CachedValues[column] = value;
	}

	return node->CachedValues[column];
}

QVariant DICOMModelPrivate::getUncachedValue(Node* node, int column) const
{
	QString filename = this->getFirstDICOMFilename(node);

//	QString filename = DataBase->fileForInstance(node->UID);
//	qDebug() << "filename " << filename;

	DicomImageReaderPtr reader = DicomImageReader::createFromFile(filename);
	if (!reader)
		return QVariant();

	if (column==0)
	{
		return this->getName(node->Type, reader);
	}
	if (column==1)
	{
		return this->getTimestamp(node->Type, reader);
	}
	if (column==2)
	{
		return this->getModality(node->Type, reader);
	}
	if (column==3)
	{
		return this->getImageCount(node->Type, node->UID);
	}

	return "";
}


QString DICOMModelPrivate::getName(DICOMModel::IndexType type, DicomImageReaderPtr reader) const
{
	QString retval;
	if (type == DICOMModel::PatientType)
	{
		retval = reader->getPatientName();
	}
	else if (type == DICOMModel::StudyType)
	{
		retval = reader->item()->GetElementAsString(DCM_StudyDescription);
	}
	else if (type == DICOMModel::SeriesType)
	{
		retval = reader->item()->GetElementAsString(DCM_SeriesDescription);
	}

	if (retval.isEmpty())
		retval = "No description";

	return retval;
}

QString DICOMModelPrivate::getTimestamp(DICOMModel::IndexType type, DicomImageReaderPtr reader) const
{
	QString format_date("yyyy-MM-dd");
	QString format_time("hh:mm");

	QString retval;
	if (type == DICOMModel::PatientType)
	{
		retval = reader->item()->GetElementAsDate(DCM_PatientBirthDate).toString(format_date);
	}
	else if (type == DICOMModel::StudyType)
	{
		QString date = reader->item()->GetElementAsDate(DCM_StudyDate).toString(format_date);
		QString time = reader->item()->GetElementAsTime(DCM_StudyTime).toString(format_time);
		retval = QString("%1 %2").arg(date).arg(time);
	}
	else if (type == DICOMModel::SeriesType)
	{
		QString date = reader->item()->GetElementAsDate(DCM_SeriesDate).toString(format_date);
		QString time = reader->item()->GetElementAsTime(DCM_SeriesTime).toString(format_time);
		retval = QString("%1 %2").arg(date).arg(time);
	}

	return retval;
}

QString DICOMModelPrivate::getModality(DICOMModel::IndexType type, DicomImageReaderPtr reader) const
{
	QString retval;
	if (type == DICOMModel::PatientType)
	{
	}
	else if (type == DICOMModel::StudyType)
	{
	}
	else if (type == DICOMModel::SeriesType)
	{
		retval = reader->item()->GetElementAsString(DCM_Modality);
	}

	return retval;
}

int DICOMModelPrivate::getFrameCountForSeries(QString series) const
{
	QString seriesDescription;
	int frameCount = 0;
	QStringList files = DataBase->filesForSeries(series);
	for (unsigned i=0; i<files.size(); ++i)
	{
		DicomImageReaderPtr reader = DicomImageReader::createFromFile(files[i]);
		if (!reader)
			continue;
		frameCount += reader->getNumberOfFrames();
		seriesDescription = reader->item()->GetElementAsString(DCM_SeriesDescription);
	}
	return frameCount;
//	std::cout << QString("%1 frames for series %2").arg(frameCount).arg(seriesDescription) << std::endl;
}

QString DICOMModelPrivate::getImageCount(DICOMModel::IndexType type, QString series) const
{
	QString retval;
	if (type == DICOMModel::PatientType)
	{
	}
	else if (type == DICOMModel::StudyType)
	{
	}
	else if (type == DICOMModel::SeriesType)
	{
		retval = QString("%1").arg(this->getFrameCountForSeries(series));
	}

	return retval;
}

void DICOMModelPrivate::fetchChildren(const QModelIndex& indexValue)
{
//	qDebug() << "DICOMModelPrivate::fetchChildren";
  Q_Q(DICOMModel);
  Node* node = this->nodeFromIndex(indexValue);

  if (!node || !node->canFetchMore())
	  return;
  if (!node->hasChildren())
	  return;

  q->beginInsertRows(indexValue, std::max<int>(0, node->FetchedChildren.size()-1), node->ChildrenUID.size()-1);

  for (int i=node->FetchedChildren.size(); i<node->ChildrenUID.size(); ++i)
  {
	  this->createNode(i, indexValue);
  }

  q->endInsertRows();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

DICOMModel::DICOMModel(QObject* parentObject)
  : Superclass(parentObject)
  , d_ptr(new DICOMModelPrivate(*this))
{
  Q_D(DICOMModel);
  d->init();
}

//------------------------------------------------------------------------------
DICOMModel::~DICOMModel()
{
}

//------------------------------------------------------------------------------
bool DICOMModel::canFetchMore ( const QModelIndex & parentValue ) const
{
  Q_D(const DICOMModel);
  Node* node = d->nodeFromIndex(parentValue);
//  /*qDebug()*/ << "DICOMModel::canFetchMore " << node ;
  if (!node)
	  return false;
  return node->canFetchMore();
}

//------------------------------------------------------------------------------
int DICOMModel::columnCount ( const QModelIndex & _parent ) const
{
  Q_D(const DICOMModel);
  Q_UNUSED(_parent);
	return d->Headers.size();
//  return d->RootNode != 0 ? d->Headers.size() : 0;
}

//------------------------------------------------------------------------------
QVariant DICOMModel::data ( const QModelIndex & dataIndex, int role ) const
{
	Q_D(const DICOMModel);

	if ( role == UIDRole )
	{
		Node* node = d->nodeFromIndex(dataIndex);
		return node ? node->UID : QString() ;
	}
	else if ( role == TypeRole )
	{
		Node* node = d->nodeFromIndex(dataIndex);
		return node ? node->Type : 0;
	}
	else if (( role==Qt::DisplayRole )||( role==Qt::EditRole ))
	{
		Node* node = d->nodeFromIndex(dataIndex);
		return node ? d->getValue(node, dataIndex.column()) : QVariant();
	}

	return QVariant();
}

//------------------------------------------------------------------------------
void DICOMModel::fetchMore ( const QModelIndex & parentValue )
{
	Q_D(DICOMModel);
	d->fetchChildren(parentValue);
}

//------------------------------------------------------------------------------
bool DICOMModel::hasChildren ( const QModelIndex & parentIndex ) const
{
	Q_D(const DICOMModel);
	// only items in the first columns have index, shortcut the following for
	// speed issues.
	if (parentIndex.column() > 0)
		return false;

	Node* node = d->nodeFromIndex(parentIndex);
	if (!node)
		return false;

	return node->hasChildren();
}

//------------------------------------------------------------------------------
QVariant DICOMModel::headerData(int section, Qt::Orientation orientation, int role)const
{
	Q_D(const DICOMModel);

	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		if (section < 0 || section >= d->Headers.size())
			return QVariant();

		return d->Headers[section];
	}

	return QVariant("");
}

//------------------------------------------------------------------------------
QModelIndex DICOMModel::index ( int row, int column, const QModelIndex & parentIndex ) const
{
	Q_D(const DICOMModel);

	if (d->RootNode == 0 || parentIndex.column() > 0) // only the first column has children
		return QModelIndex();

	Node* parentNode = d->nodeFromIndex(parentIndex);
	NodePtr node = parentNode->getFetchedChildForRow(row);

	return this->createIndex(row, column, node.get());
}

//------------------------------------------------------------------------------
QModelIndex DICOMModel::parent ( const QModelIndex & indexValue ) const
{
	Q_D(const DICOMModel);
	if (!indexValue.isValid())
		return QModelIndex();

	Node* node = d->nodeFromIndex(indexValue);
	Q_ASSERT(node);
	Node* parentNode = node->Parent;

	if (parentNode == 0)
		return QModelIndex(); // node is root
	if (parentNode == d->RootNode.get())
		return QModelIndex();

	return this->createIndex(parentNode->Row, 0, parentNode);
}

//------------------------------------------------------------------------------
int DICOMModel::rowCount ( const QModelIndex & parentValue ) const
{
	Q_D(const DICOMModel);
	if (d->RootNode == 0 || parentValue.column() > 0)
	{
		return 0;
	}
	Node* node = d->nodeFromIndex(parentValue);
	Q_ASSERT(node);
	// Returns the amount of rows currently cached on the client.
//	qDebug() << "node ? node->FetchedChildren.size() : " << (node ? node->FetchedChildren.size() : 0);

	return node ? node->FetchedChildren.size() : 0;
}

//------------------------------------------------------------------------------
void DICOMModel::setDatabase(QSharedPointer<ctkDICOMDatabase> db)
{
  Q_D(DICOMModel);

  this->beginResetModel();
  d->DataBase = db;

  d->RootNode.reset();
//  delete d->RootNode;
//  d->RootNode = 0;

//  if (!d->DataBase->patients().empty())
//  {
	  d->RootNode = d->createNode(-1, QModelIndex());
//  }

  this->endResetModel();
}


//------------------------------------------------------------------------------
DICOMModel::IndexType  DICOMModel::endLevel()const
{
  Q_D(const DICOMModel);
	// deprecated
  return SeriesType;
}

//------------------------------------------------------------------------------
void DICOMModel::setEndLevel(DICOMModel::IndexType level)
{
  Q_D(DICOMModel);
	// deprecated
}

//------------------------------------------------------------------------------
void DICOMModel::reset()
{
  Q_D(DICOMModel);
  // this could probably be done in a more elegant way
  this->setDatabase(d->DataBase);
}

//------------------------------------------------------------------------------
} //namespace cx


