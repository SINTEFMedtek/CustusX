/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
