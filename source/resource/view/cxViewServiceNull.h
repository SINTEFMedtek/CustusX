/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWSERVICENULL_H_
#define CXVIEWSERVICENULL_H_

#include "cxResourceVisualizationExport.h"

#include "cxViewService.h"

namespace cx
{
class cxResourceVisualization_EXPORT ViewServiceNull : public ViewService
{
public:
	ViewServiceNull();

	virtual ViewPtr get3DView(int group = 0, int index = 0);

	virtual int getActiveGroupId() const;
	virtual ViewGroupDataPtr getGroup(int groupIdx) const;
	virtual void setRegistrationMode(REGISTRATION_STATUS mode);

	virtual void autoShowData(DataPtr data);
	virtual void enableRender(bool val);
	virtual bool renderingIsEnabled() const;

    virtual QWidget* createLayoutWidget(QWidget* parent, int index);
    virtual QWidget* getLayoutWidget(int index);
    virtual QString getActiveLayout(int widgetIndex) const;
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

	virtual bool isNull();

public slots:
    virtual void aboutToStop(){}

private:
	void printWarning() const;
	QActionGroup* mActionGroup;

};
} //cx
#endif // CXVIEWSERVICENULL_H_
