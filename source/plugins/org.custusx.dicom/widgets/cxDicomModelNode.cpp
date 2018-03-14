/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDicomModelNode.h"

namespace cx
{

NodePtr DicomModelNode::NullNode;

NodePtr DicomModelNode::getNullNode()
{
	if (!NullNode)
	{
		NullNode.reset(new NullDicomModelNode);
		NullNode->Parent = NullNode.get();
//		NullNode->Row = -1;
	}

	return NullNode;
}

NodePtr DicomModelNode::createNode(int row, DicomModelNode* parent, QSharedPointer<ctkDICOMDatabase> dataBase)
{
	if (!dataBase)
		return DicomModelNode::getNullNode();

	NodePtr node;
	if (row == -1)
	{
		node.reset(new RootDicomModelNode);
		node->Parent = DicomModelNode::getNullNode().get();
	}
	else
	{
		DICOMModel::IndexType type = DICOMModel::IndexType(parent->getType() + 1);

		if (type==DICOMModel::PatientType)
			node.reset(new PatientDicomModelNode);
		if (type==DICOMModel::StudyType)
			node.reset(new StudyDicomModelNode);
		if (type==DICOMModel::SeriesType)
			node.reset(new SeriesDicomModelNode);

		parent->FetchedChildren.push_back(node);
		node->Parent = parent;
		node->UID = parent->ChildrenUID[row];
	}

//	node->Row = row;
	node->DataBase = dataBase;

	node->fillChildrenUids();

	return node;
}

DicomModelNode::DicomModelNode() :
//	Row(-1),
	Parent(0)
{

}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

QStringList DicomModelNode::getHeaders() const
{
	QStringList retval;
	retval << "Name";
	retval << "Date";
	retval << "Modality";
	retval << "Images";
//	retval << "Info";
	return retval;
}

bool DicomModelNode::canFetchMore() const
{
	return FetchedChildren.size() != ChildrenUID.size();
}

bool DicomModelNode::hasChildren() const
{
	return !ChildrenUID.empty();
}

NodePtr DicomModelNode::getFetchedChildForRow(int row) const
{
	if (row < this->FetchedChildren.size())
		return this->FetchedChildren[row];
	return DicomModelNode::getNullNode();
}

DicomImageReaderPtr DicomModelNode::createReader() const
{
	QString filename = this->getFirstDICOMFilename();
	return DicomImageReader::createFromFile(filename);
}

QVariant DicomModelNode::getValue(int column) const
{
	if (!this->CachedValues.count(column))
	{
		QVariant value = this->getUncachedValue(column);
		this->CachedValues[column] = value;
	}

	return this->CachedValues[column];
}

QVariant DicomModelNode::getUncachedValue(int column) const
{
	if (column==0)
		return this->getName();
	if (column==1)
		return this->getTimestamp();
	if (column==2)
		return this->getModality();
	if (column==3)
		return this->getImageCount();
	return QVariant();
}

void DicomModelNode::removeChild(int index)
{
	if (index < ChildrenUID.size())
		ChildrenUID.removeAt(index);
	if (index < FetchedChildren.size())
	{
		std::vector<NodePtr>::iterator iter = FetchedChildren.begin();
		std::advance(iter, index);
		FetchedChildren.erase(iter);
	}
	CachedValues.erase(index);
}

int DicomModelNode::getRow() const
{
	const std::vector<NodePtr>& siblings = Parent->getFetchedChildren();
	for (int i=0; i<siblings.size(); ++i)
		if (siblings[i].get()==this)
			return i;
	return -1;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

void RootDicomModelNode::fillChildrenUids()
{
//	qDebug() << "using db " << DataBase.data();
//	qDebug() << "using patients " << DataBase->patients();

	this->ChildrenUID << DataBase->patients();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

void PatientDicomModelNode::fillChildrenUids()
{
	this->ChildrenUID << DataBase->studiesForPatient(this->UID);
}

QVariant PatientDicomModelNode::getName() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	QString retval = reader->getPatientName();
	if (retval.isEmpty())
		return this->getDefaultName();
	return retval;
}

QVariant PatientDicomModelNode::getTimestamp() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	return reader->item()->GetElementAsDate(DCM_PatientBirthDate).toString(this->format_date());
}

QString PatientDicomModelNode::getFirstDICOMFilename() const
{
	QStringList studies = DataBase->studiesForPatient(this->UID);
	if (studies.empty())
		return "";
	QStringList series = DataBase->seriesForStudy(studies[0]);
	if (series.empty())
		return "";
	QStringList files = DataBase->filesForSeries(series[0]);
	if (files.empty())
		return "";
	return files[0];
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

void StudyDicomModelNode::fillChildrenUids()
{
	this->ChildrenUID << DataBase->seriesForStudy(this->UID);
}

QVariant StudyDicomModelNode::getName() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	QString retval = reader->item()->GetElementAsString(DCM_StudyDescription);
	if (retval.isEmpty())
		return this->getDefaultName();
	return retval;
}

QVariant StudyDicomModelNode::getTimestamp() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	QString date = reader->item()->GetElementAsDate(DCM_StudyDate).toString(this->format_date());
	QString time = reader->item()->GetElementAsTime(DCM_StudyTime).toString(this->format_time());
	return QString("%1 %2").arg(date).arg(time);
}

QString StudyDicomModelNode::getFirstDICOMFilename() const
{
	QStringList series = DataBase->seriesForStudy(this->UID);
	if (series.empty())
		return "";
	QStringList files = DataBase->filesForSeries(series[0]);
	if (files.empty())
		return "";
	return files[0];
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


QVariant SeriesDicomModelNode::getName() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	QString retval = reader->item()->GetElementAsString(DCM_SeriesDescription);
	if (retval.isEmpty())
		return this->getDefaultName();
	return retval;
}

QVariant SeriesDicomModelNode::getTimestamp() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	QString date = reader->item()->GetElementAsDate(DCM_SeriesDate).toString(this->format_date());
	QString time = reader->item()->GetElementAsTime(DCM_SeriesTime).toString(this->format_time());
	return QString("%1 %2").arg(date).arg(time);
}

QVariant SeriesDicomModelNode::getModality() const
{
	DicomImageReaderPtr reader = this->createReader();
	if (!reader)
		return QVariant();
	return reader->item()->GetElementAsString(DCM_Modality);
}

QVariant SeriesDicomModelNode::getImageCount() const
{
	return QString("%1").arg(this->getFrameCountForSeries(this->UID));
}

QString SeriesDicomModelNode::getFirstDICOMFilename() const
{
	QStringList files = DataBase->filesForSeries(this->UID);
	if (files.empty())
		return "";
	return files[0];
}

int SeriesDicomModelNode::getFrameCountForSeries(QString series) const
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
}


} // namespace cx

