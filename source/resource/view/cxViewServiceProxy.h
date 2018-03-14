/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWSERVICEPROXY_H_
#define CXVIEWSERVICEPROXY_H_

#include "cxResourceVisualizationExport.h"

#include "cxViewService.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

class cxResourceVisualization_EXPORT ViewServiceProxy : public ViewService
{
public:
	static ViewServicePtr create(ctkPluginContext *pluginContext);
	ViewServiceProxy(ctkPluginContext *pluginContext);

	virtual ViewPtr get3DView(int group = 0, int index = 0);

	virtual int getActiveGroupId() const;
	virtual ViewGroupDataPtr getGroup(int groupIdx) const;
	virtual void setRegistrationMode(REGISTRATION_STATUS mode);

	virtual void autoShowData(DataPtr data);
	virtual void enableRender(bool val);
	virtual bool renderingIsEnabled() const;

    virtual QWidget* createLayoutWidget(QWidget* parent, int index);
    virtual QWidget* getLayoutWidget(int index);
	virtual QString getActiveLayout(int widgetIndex=0) const;
	virtual void setActiveLayout(const QString& uid, int widgetIndex);
	virtual ClippersPtr getClippers();
	virtual InteractiveCropperPtr getCropper();
	virtual CyclicActionLoggerPtr getRenderTimer();
	virtual NavigationPtr getNavigation(int group = 0);
	virtual LayoutRepositoryPtr getLayoutRepository();
	virtual CameraControlPtr getCameraControl();
	virtual QActionGroup* getInteractorStyleActionGroup();
	virtual void centerToImageCenterInActiveViewGroup();
	virtual void setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx);
	virtual void zoomCamera3D(int viewGroup3DNumber, int zoomFactor);
	virtual void addDefaultLayout(LayoutData layoutData);
	virtual void enableContextMenuForViews(bool enable=true);

	bool isNull();

public slots:
    virtual void aboutToStop();

private:
	void initServiceListener();
	void onServiceAdded(ViewService* service);
	void onServiceRemoved(ViewService *service);

	ctkPluginContext *mPluginContext;
    ViewServicePtr mViewService;
	boost::shared_ptr<ServiceTrackerListener<ViewService> > mServiceListener;
};
} //cx
#endif // CXVIEWSERVICEPROXY_H_
