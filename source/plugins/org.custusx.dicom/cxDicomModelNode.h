// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXDICOMMODELNODE_H
#define CXDICOMMODELNODE_H

#include "boost/shared_ptr.hpp"
#include <vector>
#include "cxDICOMModel.h"
#include "ctkDICOMDatabase.h"
#include "cxDicomImageReader.h"

namespace cx
{
typedef boost::shared_ptr<class DicomModelNode> NodePtr;

NodePtr createNode(int row, DicomModelNode* parent, QSharedPointer<ctkDICOMDatabase> dataBase);

/** One node representing one of <root/patient/study/series>
  *
  * Intended for use by cx::DICOMModel
  * Children are constructed lazily, using the fetchMore system
  * from QAbstractItemModel.
  *
  * \ingroup org_custusx_dicom
  * \date 2014-05-27
  * \author Christian Askeland
  */
class DicomModelNode
{
public:
	~DicomModelNode() {}
	DICOMModel::IndexType Type;
	DicomModelNode*                           Parent;
	std::vector<NodePtr>            FetchedChildren; ///< all children currently loaded (filled by fetchMore())
	QStringList                     ChildrenUID; ///< uids of all loaded and unloaded children.
	int                             Row;
	QString                         UID;
	std::map<int, QVariant>         CachedValues;
	QSharedPointer<ctkDICOMDatabase> DataBase;

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

	virtual DICOMModel::IndexType getType() const = 0;
	virtual void fillChildrenUids() = 0;
	//  void initialize();

	//	virtual QVariant getName() = 0;
	//	virtual QVariant getTimestamp() = 0;
	//	virtual QVariant getModality() = 0;
	//	virtual QVariant getImageCount() = 0;
	virtual QVariant getName() const { return QVariant(); }
	virtual QVariant getTimestamp() const { return QVariant(); }
	virtual QVariant getModality() const { return QVariant(); }
	virtual QVariant getImageCount() const { return QVariant(); }

	QVariant getDefaultName() const { return "No description"; }
	QString format_date() const { return "yyyy-MM-dd"; }
	QString format_time() const { return "hh:mm"; }

	DicomImageReaderPtr createReader() const
	{
		QString filename = this->getFirstDICOMFilename();
		return DicomImageReader::createFromFile(filename);
	}
	virtual QString getFirstDICOMFilename() const { return ""; }

};

class RootDicomModelNode : public DicomModelNode
{
public:
	RootDicomModelNode() {}
	~RootDicomModelNode() {}

	//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::RootType; }
	virtual void fillChildrenUids()
	{
		this->ChildrenUID << DataBase->patients();
	}
};


/**
  *
  */
class PatientDicomModelNode : public DicomModelNode
{
public:
	PatientDicomModelNode() {}
	~PatientDicomModelNode() {}

	//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::PatientType; }
	virtual void fillChildrenUids()
	{
		this->ChildrenUID << DataBase->studiesForPatient(this->UID);
	}
	virtual QVariant getName() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		QString retval = reader->getPatientName();
		if (retval.isEmpty())
			return this->getDefaultName();
		return retval;
	}
	virtual QVariant getTimestamp() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		return reader->item()->GetElementAsDate(DCM_PatientBirthDate).toString(this->format_date());
	}

	virtual QString getFirstDICOMFilename() const
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

};

/**
  *
  */
class StudyDicomModelNode : public DicomModelNode
{
public:
	StudyDicomModelNode() {}
	~StudyDicomModelNode() {}

	//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::StudyType; }
	virtual void fillChildrenUids()
	{
		this->ChildrenUID << DataBase->seriesForStudy(this->UID);
	}
	virtual QVariant getName() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		QString retval = reader->item()->GetElementAsString(DCM_StudyDescription);
		if (retval.isEmpty())
			return this->getDefaultName();
		return retval;
	}
	virtual QVariant getTimestamp() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		QString date = reader->item()->GetElementAsDate(DCM_StudyDate).toString(this->format_date());
		QString time = reader->item()->GetElementAsTime(DCM_StudyTime).toString(this->format_time());
		return QString("%1 %2").arg(date).arg(time);
	}

	virtual QString getFirstDICOMFilename() const
	{
		QStringList series = DataBase->seriesForStudy(this->UID);
		if (series.empty())
			return "";
		QStringList files = DataBase->filesForSeries(series[0]);
		if (files.empty())
			return "";
		return files[0];
	}

};

/**
  *
  */
class SeriesDicomModelNode : public DicomModelNode
{
public:
	SeriesDicomModelNode() {}
	~SeriesDicomModelNode() {}

	//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::SeriesType; }
	virtual void fillChildrenUids()
	{
	}
	virtual QVariant getName() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		QString retval = reader->item()->GetElementAsString(DCM_SeriesDescription);
		if (retval.isEmpty())
			return this->getDefaultName();
		return retval;
	}
	virtual QVariant getTimestamp() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		QString date = reader->item()->GetElementAsDate(DCM_SeriesDate).toString(this->format_date());
		QString time = reader->item()->GetElementAsTime(DCM_SeriesTime).toString(this->format_time());
		return QString("%1 %2").arg(date).arg(time);
	}
	virtual QVariant getModality() const
	{
		DicomImageReaderPtr reader = this->createReader();
		if (!reader)
			return QVariant();
		return reader->item()->GetElementAsString(DCM_Modality);
	}
	virtual QVariant getImageCount() const
	{
		return QString("%1").arg(this->getFrameCountForSeries(this->UID));
	}

	virtual QString getFirstDICOMFilename() const
	{
		QStringList files = DataBase->filesForSeries(this->UID);
		if (files.empty())
			return "";
		return files[0];
	}

	int getFrameCountForSeries(QString series) const
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


};

} // namespace cx

#endif // CXDICOMMODELNODE_H
