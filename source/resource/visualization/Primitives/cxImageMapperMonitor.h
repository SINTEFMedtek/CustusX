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
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;

/** Helper class that uses the stored clip planes in a Image to clip it in a mapper.
 *
 * Used internally by VolumetricRep - No need to use this elsewhere!
 *
 * \ingroup cx_resource_visualization
 * \author christiana
 */
class ImageMapperMonitor : public QObject
{
	Q_OBJECT
public:
	static ImageMapperMonitorPtr create(vtkVolumePtr volume, ImagePtr image);
	~ImageMapperMonitor();

protected slots:
	virtual void applyCropping();
	virtual void clearClipping();
	virtual void applyClipping();

private slots:
	void clipPlanesChangedSlot();

protected:
	ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image);
	vtkVolumePtr mVolume;
	ImagePtr mImage;
	void init();

private:
	std::vector<vtkPlanePtr> mPlanes;
	vtkVolumeMapperPtr getMapper();

};

} // namespace cx
#endif // CXIMAGEMAPPERMONITOR_H
