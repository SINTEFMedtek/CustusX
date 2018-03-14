/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXINTERACTIVECLIPPER_H_
#define CXINTERACTIVECLIPPER_H_

#include "cxResourceVisualizationExport.h"

#include <vector>
#include <QObject>
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
class QDomNode;

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
	QString getDataUids();
	void setDataUids(QString uids);
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
	void parseXml(QDomNode dataNode);
	void addXml(QDomNode &dataNode);
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
