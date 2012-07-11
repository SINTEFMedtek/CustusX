// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCSLICEPLANECLIPPER_H_
#define SSCSLICEPLANECLIPPER_H_

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

/*
 * \date Aug 20, 2010
 * \author christiana
 */

/**\brief Clip several 3D volumes using a SliceProxy.
 *
 * Several 3D volumes, represented as VolumetricBaseRep, are decorated with
 * a clip plane defined by a SliceProxy.
 *
 * \ingroup sscRep
 */
class SlicePlaneClipper : public QObject
{
	Q_OBJECT
public:
	typedef std::set<ssc::VolumetricBaseRepPtr> VolumesType;

	static SlicePlaneClipperPtr New();
	~SlicePlaneClipper();
	void setSlicer(ssc::SliceProxyPtr slicer);
	void clearVolumes();
	void addVolume(ssc::VolumetricBaseRepPtr volume);
	void removeVolume(ssc::VolumetricBaseRepPtr volume);
	VolumesType getVolumes();
	ssc::SliceProxyPtr getSlicer();
	void setInvertPlane(bool on);
	bool getInvertPlane() const;
	vtkPlanePtr getClipPlaneCopy();

	void saveClipPlaneToVolume(); ///< save the current clip to image
	void clearClipPlanesInVolume(); ///< clear all saved clips in the image.

	private slots:
	void changedSlot();
	void volumeRepChangedSlot();
private:
	void addClipPlane(ssc::VolumetricBaseRepPtr volume, vtkPlanePtr clipPlane);
	SlicePlaneClipper();
	void updateClipPlane();
	ssc::Vector3D getUnitNormal() const;
	ssc::SliceProxyPtr mSlicer;
	VolumesType mVolumes;
	bool mInvertPlane;

	vtkPlanePtr mClipPlane;
//  std::vector<vtkPlanePtr> mFixedPlanes;
};

/**Helper class that uses the stored clip planes in a Image to clip it in a mapper.
 *
 * Used internally by VolumetricRep - No need to use this elsewhere!
 */
class ImageMapperMonitor : public QObject
{
	Q_OBJECT
public:
	ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image);
	~ImageMapperMonitor();

private slots:
	void clipPlanesChangedSlot();
	void cropBoxChangedSlot();
private:
	//ssc::VolumetricRepPtr mVolume;
	vtkVolumePtr mVolume;
	ImagePtr mImage;
	std::vector<vtkPlanePtr> mPlanes;
	vtkVolumeMapperPtr getMapper();

	void clearClipPlanes();
	void fillClipPlanes();
};

} // namespace ssc

#endif /* SSCSLICEPLANECLIPPER_H_ */
