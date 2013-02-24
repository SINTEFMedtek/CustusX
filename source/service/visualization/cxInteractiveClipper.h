// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXINTERACTIVECLIPPER_H_
#define CXINTERACTIVECLIPPER_H_

#include <vector>
#include <QObject>
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"

namespace cx
{
/**
* \file
* \addtogroup cxServiceVisualization
* @{
*/

/** Helper class for clipping the active volume using a specific slice plane.
 *  The visible slice planes are the only ones allowed for clipping.
 *  \date Aug 24, 2010
 *  \author christiana
 */
class InteractiveClipper: public QObject
{
Q_OBJECT
public:
	InteractiveClipper();

	void setSlicePlane(ssc::PLANE_TYPE plane);
	void saveClipPlaneToVolume(); ///< save the current clip to image
	void clearClipPlanesInVolume(); ///< clear all saved clips in the image.
	ssc::PLANE_TYPE getSlicePlane();
	bool getUseClipper() const;
	bool getInvertPlane() const;
	std::vector<ssc::PLANE_TYPE> getAvailableSlicePlanes() const;
	ssc::ImagePtr getImage() const;
	void setImage(ssc::ImagePtr image);

signals:
	void changed();
public slots:
	void useClipper(bool on);
	void invertPlane(bool on);
private slots:
	void changedSlot();
	void dominantToolChangedSlot();

private:
	ssc::PLANE_TYPE getPlaneType();
	ssc::SlicePlaneClipperPtr mSlicePlaneClipper;
	ssc::SlicePlanesProxyPtr mSlicePlanesProxy;
	bool mUseClipper;
	ssc::ImagePtr mImage;
};
typedef boost::shared_ptr<InteractiveClipper> InteractiveClipperPtr;

/**
* @}
*/
} // namespace cx

#endif /* CXINTERACTIVECLIPPER_H_ */
