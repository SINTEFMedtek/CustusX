/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXTREEREPOSITORY_H
#define CXTREEREPOSITORY_H

#include <vector>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include <QWidget>
#include "cxForwardDeclarations.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxXmlOptionItem.h"

namespace cx
{

class TreeNode;
class TreeRepository;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;
typedef boost::shared_ptr<TreeRepository> TreeRepositoryPtr;
typedef boost::shared_ptr<class StringListProperty> StringListPropertyPtr;
typedef boost::weak_ptr<class TreeRepository> TreeRepositoryWeakPtr;
typedef boost::shared_ptr<class WidgetTypeRepository> WidgetTypeRepositoryPtr;

class WidgetTypeRepository
{
public:
	template<class WIDGET>
	WIDGET* find()
	{
		for (unsigned i=0; i<mWidgets.size(); ++i)
		{
			WIDGET* w = dynamic_cast<WIDGET*>(mWidgets[i].data());
			if (w)
				return w;
		}
		return NULL;
	}
	void add(QWidget* widget);

private:
	typedef QPointer<QWidget> QWidgetPtr;
	std::vector<QWidgetPtr> mWidgets;
};

class TreeRepository : public QObject
{
	Q_OBJECT
public:
	static TreeRepositoryPtr create(XmlOptionFile options, VisServicesPtr services);
	~TreeRepository();

	std::vector<TreeNodePtr> getNodes();
	TreeNodePtr getNode(QString uid);

	// utility methods for accessing the nodes:
	TreeNodePtr getTopNode();
	TreeNodePtr getNodeForGroup(QString groupname);
	VisServicesPtr getServices();

	WidgetTypeRepositoryPtr getWidgetTypeRepository();

	QString getMode() const;
	StringPropertyPtr getModeProperty() { return mModeProperty; }
	QStringList getVisibleNodeTypes() const;
	StringListPropertyPtr getVisibilityProperty() { return mVisibilityProperty; }
	QStringList getAllNodeTypes() const;

public slots:
	void update();
	void invalidate();

signals:
	void changed(TreeNode* node=NULL);
	void invalidated();
	void loaded();

private:
	void createVisibilityProperty();
	void createModeProperty();
	void onChanged();

	std::vector<TreeNodePtr> mNodes;
	TreeRepositoryWeakPtr mSelf;
	bool mInvalid;
	VisServicesPtr mServices;

	StringPropertyPtr mModeProperty;
	StringListPropertyPtr mVisibilityProperty;

	XmlOptionFile mOptions;
	WidgetTypeRepositoryPtr mWidgetTypeRepository;

	explicit TreeRepository(XmlOptionFile options, VisServicesPtr services);
	void rebuild();
	void insertTopNode();
	void insertDataNode(DataPtr data);
	void insertSpaceNode(CoordinateSystem space);
	void insertToolNode(ToolPtr tool);
	void insertGroupNode(QString groupname);
	void appendNode(TreeNode* rawNode);
	void startListen();
	void stopListen();
};

} // namespace cx

#endif // CXTREEREPOSITORY_H
