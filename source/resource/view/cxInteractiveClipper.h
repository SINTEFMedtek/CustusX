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

#ifndef CXINTERACTIVECLIPPER_H_
#define CXINTERACTIVECLIPPER_H_

#include "cxResourceVisualizationExport.h"

#include <vector>
#include <QObject>
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;

/**
* \file
* \addtogroup cx_resource_view
* @{
*/

/** Helper class for clipping the active volume using a specific slice plane.
 *  The visible slice planes are the only ones allowed for clipping.
 *  \date Aug 24, 2010
 *  \author christiana
 */
class cxResourceVisualization_EXPORT InteractiveClipper: public QObject
{
Q_OBJECT
public:
	InteractiveClipper(CoreServicesPtr services);

	void setSlicePlane(PLANE_TYPE plane);
	void saveClipPlaneToVolume(); ///< save the current clip to image
	void clearClipPlanesInVolume(); ///< clear all saved clips in the image.
	PLANE_TYPE getSlicePlane();
	bool getUseClipper() const;
	bool getInvertPlane() const;
	std::vector<PLANE_TYPE> getAvailableSlicePlanes() const;
	DataPtr getData() const;
	void setData(DataPtr data);

	void addData(DataPtr data);
	void removeData(DataPtr data);
	bool exists(DataPtr data);
	std::map<QString, DataPtr> getDatas();
	void setTool(ToolPtr tool);
	void useActiveTool(bool on);
signals:
	void changed();
public slots:
	void useClipper(bool on);
	void invertPlane(bool on);
private slots:
	void changedSlot();
	void activeToolChangedSlot();

protected:
	PLANE_TYPE getPlaneType();
	SlicePlaneClipperPtr mSlicePlaneClipper;
	SlicePlanesProxyPtr mSlicePlanesProxy;
	bool mUseClipper;
	DataPtr mData;
	CoreServicesPtr mServices;
	std::map<QString, DataPtr> mDatas;
	bool mUseActiveTool;
	void removeAllInterActiveClipPlanes();
	void addAllInteractiveClipPlanes();
	void updateClipPlanesInData();
};
typedef boost::shared_ptr<InteractiveClipper> InteractiveClipperPtr;

/**
* @}
*/
} // namespace cx

#endif /* CXINTERACTIVECLIPPER_H_ */
