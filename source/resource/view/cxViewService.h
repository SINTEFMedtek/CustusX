/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWSERVICE_H_
#define CXVIEWSERVICE_H_

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "cxData.h"
#include "cxLayoutData.h"
#include "cxEnumConverter.h"
#include "cxForwardDeclarations.h"

#define ViewService_iid "cx::ViewService"

class QActionGroup;

namespace cx
{

enum cxResourceVisualization_EXPORT CAMERA_STYLE_TYPE
{
	cstDEFAULT_STYLE, cstTOOL_STYLE, cstANGLED_TOOL_STYLE, cstUNICAM_STYLE, cstCOUNT
};

} //cx

SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResourceVisualization_EXPORT, cx, CAMERA_STYLE_TYPE);

namespace cx
{

/** \brief Visualization services
 *
 *  \ingroup cx_resource_view
 *  \date 2014-09-19
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT ViewService : public QObject
{
	Q_OBJECT
public:
	virtual ViewPtr get3DView(int group = 0, int index = 0) = 0;
	RepContainerPtr get3DReps(int group = 0, int index = 0);

	//TODO: Remove direct access to this internal structure
	virtual int getActiveGroupId() const = 0;
	virtual ViewGroupDataPtr getGroup(int groupIdx) const = 0;
	unsigned groupCount() const;

	//All above group access needs to be replaced with functions:
	virtual void setRegistrationMode(REGISTRATION_STATUS mode) = 0;

	virtual void autoShowData(DataPtr data) = 0;
	virtual void enableRender(bool val) = 0;
	virtual bool renderingIsEnabled() const = 0;
    virtual QWidget* createLayoutWidget(QWidget* parent, int index=0) = 0; ///< Get the specified layout widget, create if necessary
    virtual QWidget* getLayoutWidget(int index) = 0; ///< Get the specified layout widget, NULL if not created
    virtual QString getActiveLayout(int widgetIndex=0) const = 0;
	virtual void setActiveLayout(const QString& uid, int widgetIndex=0) = 0;
	virtual void deactivateLayout();
	virtual ClippersPtr getClippers() = 0;
	virtual InteractiveCropperPtr getCropper() = 0;
	virtual CyclicActionLoggerPtr getRenderTimer() = 0;
	virtual NavigationPtr getNavigation(int group = 0) = 0;
	virtual LayoutRepositoryPtr getLayoutRepository() = 0;
	virtual CameraControlPtr getCameraControl() = 0;
	virtual QActionGroup* getInteractorStyleActionGroup() = 0;
	virtual void centerToImageCenterInActiveViewGroup() = 0;
	virtual void setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx) = 0;
	virtual void zoomCamera3D(int viewGroup3DNumber, int zoomFactor) = 0;
	virtual void addDefaultLayout(LayoutData layoutData) = 0;
	virtual void enableContextMenuForViews(bool enable=true) = 0;

	virtual bool isNull() = 0;
	static ViewServicePtr getNullObject();

	// extended Data interface
	ViewGroupDataPtr getActiveViewGroup();
signals:
	void fps(int number); ///< Emits number of frames per second
	void activeLayoutChanged(); ///< emitted when the active layout changes
	void activeViewChanged(); ///< emitted when the active view changes
	void renderingEnabledChanged(); ///< emitted then rendering is enabled/disabled
	void pointSampled(Vector3D p_r);
	void renderFinished();

public slots:
    virtual void aboutToStop() = 0;

};

} //cx
Q_DECLARE_INTERFACE(cx::ViewService, ViewService_iid)

#endif // CXVIEWSERVICE_H_
