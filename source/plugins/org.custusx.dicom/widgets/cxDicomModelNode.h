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
	static NodePtr createNode(int row, DicomModelNode* parent, QSharedPointer<ctkDICOMDatabase> dataBase);
	static NodePtr getNullNode();

	DicomModelNode();
	virtual ~DicomModelNode() {}

	bool canFetchMore() const;
	bool hasChildren() const;
	NodePtr getFetchedChildForRow(int row) const;

	virtual DICOMModel::IndexType getType() const = 0;
	virtual void fillChildrenUids() = 0;
	virtual QVariant getName() const { return QVariant(); }
	virtual QVariant getTimestamp() const { return QVariant(); }
	virtual QVariant getModality() const { return QVariant(); }
	virtual QVariant getImageCount() const { return QVariant(); }

	QVariant getDefaultName() const { return "No description"; }
	QString format_date() const { return "yyyy-MM-dd"; }
	QString format_time() const { return "hh:mm"; }

	DicomImageReaderPtr createReader() const;
	virtual QString getFirstDICOMFilename() const { return ""; }
	QString getUid() const { return UID; }
	int getRow() const;
	DicomModelNode* getParent() const { return Parent; }
	const std::vector<NodePtr>& getFetchedChildren() const { return FetchedChildren; }  ///< all children currently loaded (filled by fetchMore())
	QStringList getChildrenUID() const { return ChildrenUID; } ///< uids of all loaded and unloaded children.
	void removeChild(int index);

	QVariant getValue(int column) const;
	QStringList getHeaders() const;
	bool isNull() const { return this->getType()==DICOMModel::NoneType; }

protected:
	QVariant getUncachedValue(int column) const;

	DicomModelNode*                 Parent;
	std::vector<NodePtr>            FetchedChildren; ///< all children currently loaded (filled by fetchMore())
	QStringList                     ChildrenUID; ///< uids of all loaded and unloaded children.
//	int                             Row;
	QString                         UID;
	mutable std::map<int, QVariant> CachedValues;
	QSharedPointer<ctkDICOMDatabase> DataBase;
	static NodePtr NullNode;
};


/**
  *
  */
class NullDicomModelNode : public DicomModelNode
{
public:
	NullDicomModelNode() {}
	virtual ~NullDicomModelNode() {}

	virtual DICOMModel::IndexType getType() const { return DICOMModel::NoneType; }
	virtual void fillChildrenUids() {}
};

/**
  *
  */
class RootDicomModelNode : public DicomModelNode
{
public:
	RootDicomModelNode() {}
	virtual ~RootDicomModelNode() {}

	virtual DICOMModel::IndexType getType() const { return DICOMModel::RootType; }
	virtual void fillChildrenUids();
};


/**
  *
  */
class PatientDicomModelNode : public DicomModelNode
{
public:
	PatientDicomModelNode() {}
	virtual ~PatientDicomModelNode() {}

	virtual DICOMModel::IndexType getType() const { return DICOMModel::PatientType; }
	virtual void fillChildrenUids();
	virtual QVariant getName() const;
	virtual QVariant getTimestamp() const;
	virtual QString getFirstDICOMFilename() const;
};

/**
  *
  */
class StudyDicomModelNode : public DicomModelNode
{
public:
	StudyDicomModelNode() {}
	virtual ~StudyDicomModelNode() {}

	virtual DICOMModel::IndexType getType() const { return DICOMModel::StudyType; }
	virtual void fillChildrenUids();
	virtual QVariant getName() const;
	virtual QVariant getTimestamp() const;
	virtual QString getFirstDICOMFilename() const;
};

/**
  *
  */
class SeriesDicomModelNode : public DicomModelNode
{
public:
	SeriesDicomModelNode() {}
	virtual ~SeriesDicomModelNode() {}

	virtual DICOMModel::IndexType getType() const { return DICOMModel::SeriesType; }
	virtual void fillChildrenUids()	{}
	virtual QVariant getName() const;
	virtual QVariant getTimestamp() const;
	virtual QVariant getModality() const;
	virtual QVariant getImageCount() const;
	virtual QString getFirstDICOMFilename() const;
	int getFrameCountForSeries(QString series) const;
};

} // namespace cx

#endif // CXDICOMMODELNODE_H
