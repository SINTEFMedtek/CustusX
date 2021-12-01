/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include "org_custusx_core_view_Export.h"

#include <vector>
#include <QVariant>
#include <QObject>
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxViewGroupData.h"
#include "cxSyncedValue.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxImageAlgorithms.h"

typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */


typedef boost::shared_ptr<class DataViewPropertiesInteractor> DataViewPropertiesInteractorPtr;
/** Provide an action list for showing data in views.
  *
  */
class org_custusx_core_view_EXPORT DataViewPropertiesInteractor : public QObject
{
	Q_OBJECT
public:
	DataViewPropertiesInteractor(VisServicesPtr services, ViewGroupDataPtr groupData);
	void setDataViewProperties(DataViewProperties properties);

	template <class DATA>
	void addDataActionsOfType(QWidget* parent);

private slots:
	void dataActionSlot();
private:
	void addDataAction(QString uid, QWidget* parent);
	VisServicesPtr mServices;
	ViewGroupDataPtr mGroupData;
	DataViewProperties mProperties;

	QString mLastDataActionUid;
};

template <class DATA>
void DataViewPropertiesInteractor::addDataActionsOfType(QWidget* parent)
{
	//add actions to the actiongroups and the contextmenu
	std::vector< boost::shared_ptr<DATA> > sorted = sortOnGroupsAndAcquisitionTime(mServices->patient()->getDataOfType<DATA>());
	mLastDataActionUid = "________________________";
	for (typename std::vector< boost::shared_ptr<DATA> >::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
	{
		this->addDataAction((*iter)->getUid(), parent);
	}
}

/**
 * \brief Superclass for ViewWrappers.
 *
 *
 * \date 6. apr. 2010
 * \\author jbake
 */
class org_custusx_core_view_EXPORT ViewWrapper: public QObject
{
Q_OBJECT
public:
	virtual ~ViewWrapper() {}
	virtual void initializePlane(PLANE_TYPE plane) {}
	virtual ViewPtr getView() = 0;
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy) = 0;
	virtual void setViewGroup(ViewGroupDataPtr group);
	virtual void updateView();
	virtual void setSharedOpenGLContext(SharedOpenGLContextPtr sharedOpenGLContext);
	ToolPtr getControllingTool();

signals:
	void orientationChanged(ORIENTATION_TYPE type);

protected slots:
	void contextMenuSlot(const QPoint& point);
	void settingsChangedSlot(QString key);

	virtual void dataViewPropertiesChangedSlot(QString uid) = 0;
	virtual void videoSourceChangedSlot(QString uid) {}
	virtual void activeToolChangedSlot() {};

protected:
	ViewWrapper(VisServicesPtr backend);

	void connectContextMenu(ViewPtr view);
	virtual QString getDataDescription() = 0;
	virtual QString getViewDescription() = 0;
	virtual void appendToContextMenu(QMenu& contextMenu) = 0;
	QStringList getAllDataNames(DataViewProperties properties) const;
	virtual void addReps();

	ViewGroupDataPtr mGroupData;
	VisServicesPtr mServices;
	DataViewPropertiesInteractorPtr mDataViewPropertiesInteractor;
	DataViewPropertiesInteractorPtr mShow3DSlicesInteractor;
	SharedOpenGLContextPtr mSharedOpenGLContext;

private:
	DisplayTextRepPtr mPlaneTypeText;
	DisplayTextRepPtr mDataNameText;
};

/**
 * @}
 */
} //namespace cx

#endif /* CXVIEWWRAPPER_H_ */
