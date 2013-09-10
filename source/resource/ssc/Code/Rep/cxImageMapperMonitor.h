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
#ifndef CXIMAGEMAPPERMONITOR_H
#define CXIMAGEMAPPERMONITOR_H

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

/** Helper class that uses the stored clip planes in a Image to clip it in a mapper.
 *
 * Used internally by VolumetricRep - No need to use this elsewhere!
 *
 * \author christiana
 */
class ImageMapperMonitor : public QObject
{
	Q_OBJECT
public:
	ImageMapperMonitor(vtkVolumePtr volume, ssc::ImagePtr image);
	~ImageMapperMonitor();

private slots:
	void clipPlanesChangedSlot();
	void cropBoxChangedSlot();
private:
	vtkVolumePtr mVolume;
	ssc::ImagePtr mImage;
	std::vector<vtkPlanePtr> mPlanes;
	vtkVolumeMapperPtr getMapper();

	void clearClipPlanes();
	void fillClipPlanes();
};

} // namespace cx
#endif // CXIMAGEMAPPERMONITOR_H
