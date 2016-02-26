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

#ifndef CXVIEWSERVICE_H_
#define CXVIEWSERVICE_H_

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "cxData.h"
class QActionGroup;

#define ViewService_iid "cx::ViewService"

namespace cx
{

typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
typedef boost::shared_ptr<class Clippers> ClippersPtr;

typedef boost::shared_ptr<class CyclicActionLogger> CyclicActionLoggerPtr;
typedef boost::shared_ptr<class Navigation> NavigationPtr;
typedef boost::shared_ptr<class LayoutRepository> LayoutRepositoryPtr;
typedef boost::shared_ptr<class CameraControl> CameraControlPtr;
typedef boost::shared_ptr<class RepContainer> RepContainerPtr;


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
	virtual NavigationPtr getNavigation() = 0;
	virtual LayoutRepositoryPtr getLayoutRepository() = 0;
	virtual CameraControlPtr getCameraControl() = 0;
	virtual QActionGroup* createInteractorStyleActionGroup() = 0;

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

public slots:
    virtual void aboutToStop() = 0;

};

} //cx
Q_DECLARE_INTERFACE(cx::ViewService, ViewService_iid)

#endif // CXVIEWSERVICE_H_
