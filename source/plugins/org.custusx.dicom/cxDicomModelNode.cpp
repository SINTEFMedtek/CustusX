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

#include "cxDicomModelNode.h"

namespace cx
{

NodePtr createNode(int row, Node* parent, QSharedPointer<ctkDICOMDatabase> dataBase)
{
	if (!dataBase)
		return NodePtr();
	qDebug() << "database " << dataBase.data();
	Q_ASSERT(dataBase.data());
	NodePtr node;
//	Node* nodeParent = 0;
	if (row == -1)
	{// root node
		node.reset(new RootNode);
//		node->Type = DICOMModel::RootType;
		node->Parent = 0;
	}
	else
	{
		DICOMModel::IndexType type = DICOMModel::IndexType(parent->Type + 1);

		if (type==DICOMModel::PatientType)
			node.reset(new PatientNode);
		if (type==DICOMModel::StudyType)
			node.reset(new StudyNode);
		if (type==DICOMModel::SeriesType)
			node.reset(new SeriesNode);

		parent->FetchedChildren.push_back(node);
		node->Parent = parent;
		node->UID = parent->ChildrenUID[row];
	}
	node->Type = node->getType();
	node->Row = row;
	node->DataBase = dataBase;

	node->fillChildrenUids();
	qDebug() << "created node" << node->UID;
	qDebug() << "        node children " << node->ChildrenUID.join(",");

	return node;
}


} // namespace cx

