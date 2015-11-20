#include "cxBrowserWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
//#include "sscMessageManager.h"
//#include "sscDataManager.h"
#include "cxPatientModelService.h"
//#include "sscToolManager.h"
//#include "cxViewManager.h"
#include "cxViewService.h"
//#include "cxView2D.h"
//#include "cxView3D.h"
//#include "cxViewGroup.h"
#include "cxRep.h"
#include "cxViewGroupData.h"
#include "cxLegacySingletons.h"
#include "cxTypeConversions.h"
#include "cxTreeItemModel.h"
#include "cxLogger.h"

namespace cx
{

BrowserWidget::BrowserWidget(QWidget* parent) :
	BaseWidget(parent, "BrowserWidget", "Browser"),
	mVerticalLayout(new QVBoxLayout(this))
{
	this->setModified();
	mModel = new TreeItemModel(this);
	connect(mModel, SIGNAL(hasBeenReset()), this, SLOT(setModified()));
	//  mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mModel->getFilter()));

	//layout
	mTreeView = new QTreeView(this);
	mTreeView->setRootIsDecorated(false);
	mTreeView->setModel(mModel);
	mModel->setSelectionModel(mTreeView->selectionModel());
	mVerticalLayout->addWidget(mTreeView);

}

BrowserWidget::~BrowserWidget()
{
}


void BrowserWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	//  connect(patientService().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(populateTreeWidget()));
	//  connect(viewService().get(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
	//  connect(viewService().get(), SIGNAL(activeLayoutChanged()), this, SLOT(populateTreeWidget()));
	//  connect(viewService().get(), SIGNAL(activeViewChanged()), this, SLOT(populateTreeWidget()));

	//  this->populateTreeWidget();
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
	//  disconnect(patientService().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(populateTreeWidget()));
	//  disconnect(viewService().get(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
	//  disconnect(viewService().get(), SIGNAL(activeLayoutChanged()), this, SLOT(populateTreeWidget()));
	//  disconnect(viewService().get(), SIGNAL(activeViewChanged()), this, SLOT(populateTreeWidget()));
}

void BrowserWidget::prePaintEvent()
{
	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::prePaintEvent()";
	mModel->update();
	this->resetView();
}

void BrowserWidget::resetView()
{
	mTreeView->expandToDepth(4);
	mTreeView->resizeColumnToContents(0);
}


}//end namespace cx
