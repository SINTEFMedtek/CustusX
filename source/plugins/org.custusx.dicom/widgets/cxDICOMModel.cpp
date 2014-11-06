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
//#include "cxDicomImageReader.h"
#include "cxDicomModelNode.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMModel" );

Q_DECLARE_METATYPE(Qt::CheckState);
Q_DECLARE_METATYPE(QStringList);

namespace cx
{

//------------------------------------------------------------------------------
class DICOMModelPrivate
{
  Q_DECLARE_PUBLIC(DICOMModel);
protected:
  DICOMModel* const q_ptr;

public:
  DICOMModelPrivate(DICOMModel&);
  virtual ~DICOMModelPrivate();
  void fetchChildren(const QModelIndex& indexValue);
  NodePtr createNode(int row, const QModelIndex& parentValue)const;
  DicomModelNode* nodeFromIndex(const QModelIndex& indexValue)const;
  void remove(const QModelIndex& index);

  NodePtr RootNode;
  QSharedPointer<ctkDICOMDatabase> DataBase;
};

DICOMModelPrivate::DICOMModelPrivate(DICOMModel& o):q_ptr(&o)
{
	RootNode = DicomModelNode::getNullNode();
}

DICOMModelPrivate::~DICOMModelPrivate()
{
}

DicomModelNode* DICOMModelPrivate::nodeFromIndex(const QModelIndex& indexValue)const
{
	if (!indexValue.isValid())
		return this->RootNode.get();
	DicomModelNode* retval = reinterpret_cast<DicomModelNode*>(indexValue.internalPointer());
	if (retval)
		return retval;
	return DicomModelNode::getNullNode().get();
}

NodePtr DICOMModelPrivate::createNode(int row, const QModelIndex& parentValue)const
{
	DicomModelNode* parent = this->nodeFromIndex(parentValue);
	return DicomModelNode::createNode(row, parent, DataBase);
}

void DICOMModelPrivate::fetchChildren(const QModelIndex& indexValue)
{
  Q_Q(DICOMModel);

	DicomModelNode* node = this->nodeFromIndex(indexValue);

  if (!node->canFetchMore())
	  return;
  if (!node->hasChildren())
	  return;

  q->beginInsertRows(indexValue, std::max<int>(0, node->getFetchedChildren().size()-1), node->getChildrenUID().size()-1);

  for (int i=node->getFetchedChildren().size(); i<node->getChildrenUID().size(); ++i)
  {
	  this->createNode(i, indexValue);
  }

  q->endInsertRows();
}

void DICOMModelPrivate::remove(const QModelIndex& index)
{
	DicomModelNode* node = this->nodeFromIndex(index);

	qDebug() << "Remove DICOM node " << node->getValue(0).toString();

	switch (node->getType())
	{
	case DICOMModel::SeriesType :
		DataBase->removeSeries(node->getUid());
		break;
	case DICOMModel::StudyType :
		DataBase->removeStudy(node->getUid());
		break;
	case DICOMModel::PatientType :
		DataBase->removePatient(node->getUid());
		break;
	default:
		break;
	}

	node->getParent()->removeChild(node->getRow());
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
}

//------------------------------------------------------------------------------
DICOMModel::~DICOMModel()
{
}

//------------------------------------------------------------------------------
bool DICOMModel::canFetchMore ( const QModelIndex & parentValue ) const
{
  Q_D(const DICOMModel);
  DicomModelNode* node = d->nodeFromIndex(parentValue);
  return node->canFetchMore();
}

//------------------------------------------------------------------------------
int DICOMModel::columnCount ( const QModelIndex & _parent ) const
{
	Q_D(const DICOMModel);
	Q_UNUSED(_parent);
	return d->RootNode->getHeaders().size();
}

//------------------------------------------------------------------------------
QVariant DICOMModel::data ( const QModelIndex & dataIndex, int role ) const
{
	Q_D(const DICOMModel);

	if ( role == UIDRole )
	{
		DicomModelNode* node = d->nodeFromIndex(dataIndex);
		return node->getUid();
	}
	else if ( role == TypeRole )
	{
		DicomModelNode* node = d->nodeFromIndex(dataIndex);
		return node->getType();
	}
	else if (( role==Qt::DisplayRole )||( role==Qt::EditRole ))
	{
		DicomModelNode* node = d->nodeFromIndex(dataIndex);
		return node->getValue(dataIndex.column());
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
	// only items in the first columns have index, shortcut the following for speed issues.
	if (parentIndex.column() > 0)
		return false;

	DicomModelNode* node = d->nodeFromIndex(parentIndex);
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
		QStringList headers = d->RootNode->getHeaders();

		if (section < 0 || section >= headers.size())
			return QVariant();

		return headers[section];
	}

	return QVariant("");
}

//------------------------------------------------------------------------------
QModelIndex DICOMModel::index ( int row, int column, const QModelIndex & parentIndex ) const
{
	Q_D(const DICOMModel);

	if (parentIndex.column() > 0) // only the first column has children
		return QModelIndex();

	DicomModelNode* parentNode = d->nodeFromIndex(parentIndex);
	NodePtr node = parentNode->getFetchedChildForRow(row);

	if (node->isNull())
	{
		qDebug() << "DICOMModel::index() failed to get node for index";
		return QModelIndex();
	}

	return this->createIndex(row, column, node.get());
}

//------------------------------------------------------------------------------
QModelIndex DICOMModel::parent ( const QModelIndex & indexValue ) const
{
	Q_D(const DICOMModel);
	if (!indexValue.isValid())
		return QModelIndex();

	DicomModelNode* node = d->nodeFromIndex(indexValue);
	DicomModelNode* parentNode = node->getParent();

	if (parentNode->isNull())
		return QModelIndex(); // node is root
	if (parentNode == d->RootNode.get())
		return QModelIndex();

	return this->createIndex(parentNode->getRow(), 0, parentNode);
}

//------------------------------------------------------------------------------
int DICOMModel::rowCount ( const QModelIndex & parentValue ) const
{
	Q_D(const DICOMModel);
	if (parentValue.column() > 0)
		return 0;
	DicomModelNode* node = d->nodeFromIndex(parentValue);
	// Returns the amount of rows currently cached on the client.
	return node->getFetchedChildren().size();
}

//------------------------------------------------------------------------------
void DICOMModel::setDatabase(QSharedPointer<ctkDICOMDatabase> db)
{
	Q_D(DICOMModel);

	this->beginResetModel();

	d->DataBase = db;
	d->RootNode.reset();
	d->RootNode = d->createNode(-1, QModelIndex());

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

bool DICOMModel::removeRows(int row, int count, const QModelIndex& parent)
{
	Q_D(DICOMModel);

	if (count==0)
		return false;

	this->beginRemoveRows(parent, row, row+count-1);

	for (int i=row; i<row+count; ++i)
	{
		QModelIndex index = this->index(i, 0, parent);
		d->remove(index);
	}

	this->endRemoveRows();

	return true;
}

//------------------------------------------------------------------------------
} //namespace cx


