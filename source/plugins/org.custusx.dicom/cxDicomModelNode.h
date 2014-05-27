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

namespace cx
{
typedef boost::shared_ptr<class Node> NodePtr;

NodePtr createNode(int row, Node* parent, QSharedPointer<ctkDICOMDatabase> dataBase);

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
class Node
{
public:
	~Node() {}
  DICOMModel::IndexType Type;
  Node*                           Parent;
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
};

class RootNode : public Node
{
public:
	RootNode() {}
	~RootNode() {}

//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::RootType; }
	virtual void fillChildrenUids()
	{
		this->ChildrenUID << DataBase->patients();
	}
};

class PatientNode : public Node
{
public:
	PatientNode() {}
	~PatientNode() {}

//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::PatientType; }
	virtual void fillChildrenUids()
	{
		this->ChildrenUID << DataBase->studiesForPatient(this->UID);
	}
};

class StudyNode : public Node
{
public:
	StudyNode() {}
	~StudyNode() {}

//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::StudyType; }
	virtual void fillChildrenUids()
	{
		this->ChildrenUID << DataBase->seriesForStudy(this->UID);
	}
};

class SeriesNode : public Node
{
public:
	SeriesNode() {}
	~SeriesNode() {}

//	void initialize();
	virtual DICOMModel::IndexType getType() const { return DICOMModel::SeriesType; }
	virtual void fillChildrenUids()
	{
//		this->ChildrenUID << DataBase->seriesForStudy(this->UID);
	}
};

} // namespace cx

#endif // CXDICOMMODELNODE_H
