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

#ifndef CXGRAPHICALBOX_H
#define CXGRAPHICALBOX_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <QPointer>
#include <vtkSmartPointer.h>
#include "cxBoundingBox3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"


namespace cx
{
typedef vtkSmartPointer<class GraphicalBoxCallback> GraphicalBoxCallbackPtr;
typedef vtkSmartPointer<class GraphicalBoxEnableCallback> GraphicalBoxEnableCallbackPtr;
typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;

/**
* \file
* \addtogroup cx_resource_view
* @{
*/

/**Helper class for cropping the active volume using a
 * bounding box.
 * The bounding box can be set either explicitly or using
 * a vtkBoxWidget.
 */
class cxResourceVisualization_EXPORT GraphicalBox: public QObject
{
Q_OBJECT
public:
	explicit GraphicalBox();
	virtual ~GraphicalBox();
	void setRenderWindow(vtkRenderWindowPtr renderWindow); ///< adds an interactive box widget to the view. Press 'I' to show

	DoubleBoundingBox3D getBoundingBox(); ///< get BB in data space
	void setBoundingBox(const DoubleBoundingBox3D& bb_d); ///< set BB in d space
	void setMaxBoundingBox(const DoubleBoundingBox3D& bb_d); ///< set BB in d space
	void setPosition(const Transform3D& rMd); ///< set BB position
//	void resetBoundingBox(); ///< set bounding box back to initial size (entire volume)
	DoubleBoundingBox3D getMaxBoundingBox();

	bool getInteractive() const; // interactive on/off
	void setInteractive(bool on);
	bool getVisible() const; // display on/off
	void setVisible(bool on);
//	std::vector<int>  getDimensions();
signals:
	void changed(); // any internal state is changed.
public slots:
//	void showBoxWidget(bool on);
//	void useCropping(bool on);
private slots:
	void updateBoxFromWidget();
	void updateWidgetFromBox();

//	void imageCropChangedSlot();
//	void imageChangedSlot();

private:
//	void boxWasShown(bool val);
	friend class GraphicalBoxCallback;
	friend class GraphicalBoxEnableCallback;

	DoubleBoundingBox3D getCurrentBoxWidgetSize();
	void setBoxWidgetSize(const DoubleBoundingBox3D &bb_d, Transform3D rMd);
	void updateBoxWidgetInteractor();
	void initialize();

	Transform3D getBoxTransform();
	void setBoxTransform(const Transform3D& M);

	vtkRenderWindowPtr mRenderWindow;
	vtkBoxWidgetPtr mBoxWidget;
	GraphicalBoxCallbackPtr mGraphicalBoxCallback;
	GraphicalBoxEnableCallbackPtr mGraphicalBoxEnableCallback;
	GraphicalBoxEnableCallbackPtr mGraphicalBoxDisableCallback;

//	bool mShowBoxWidget;
	DoubleBoundingBox3D mBox;
	DoubleBoundingBox3D mMaxBox;
	Transform3D m_rMd; // position of box in r/world space
	bool mInteractive;
	bool mVisible;
};

typedef boost::shared_ptr<GraphicalBox> GraphicalBoxPtr;

/**
* @}
*/
} // namespace cx


#endif // CXGRAPHICALBOX_H
