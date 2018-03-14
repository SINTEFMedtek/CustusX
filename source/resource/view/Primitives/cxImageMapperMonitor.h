/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIMAGEMAPPERMONITOR_H
#define CXIMAGEMAPPERMONITOR_H

#include "cxResourceVisualizationExport.h"

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
 * \ingroup cx_resource_view
 * \author christiana
 */
class cxResourceVisualization_EXPORT ImageMapperMonitor : public QObject
{
	Q_OBJECT
public:
	static ImageMapperMonitorPtr create(vtkVolumePtr volume, ImagePtr image);
	~ImageMapperMonitor();

protected slots:
	virtual void applyCropping();
	virtual void clearClipping();
	virtual void applyClipping();

protected:
	ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image);
	vtkVolumePtr mVolume;
	ImagePtr mImage;
	void init();

private:
	std::vector<vtkPlanePtr> mPlanes;
	vtkVolumeMapperPtr getMapper();
	void clipPlanesChangedSlot();

};

} // namespace cx
#endif // CXIMAGEMAPPERMONITOR_H
