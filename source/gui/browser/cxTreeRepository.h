/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxMetricUtilities.h"
#include "cxLogger.h"

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
	~WidgetTypeRepository()
	{
	}

	template<class WIDGET>
	boost::shared_ptr<WIDGET> find()
	{
		for (unsigned i=0; i<mWidgets.size(); ++i)
		{
			boost::shared_ptr<WIDGET> w = boost::dynamic_pointer_cast<WIDGET>(mWidgets[i]);
			if (w)
				return w;
		}
		return boost::shared_ptr<WIDGET>();
	}

	boost::shared_ptr<QWidget> findMetricWidget(DataPtr data);

	void add(boost::shared_ptr<QWidget> widget);

private:
	typedef QPointer<QWidget> QWidgetPtr;
	std::vector<boost::shared_ptr<QWidget> > mWidgets;
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
