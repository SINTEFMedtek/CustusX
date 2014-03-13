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

#ifndef CXTOOLUSINGIGSTK_H_
#define CXTOOLUSINGIGSTK_H_

#include "cxToolImpl.h"

#include <limits.h>
#include <QTimer>
#include <boost/shared_ptr.hpp>
#include "cxTransform3D.h"
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"

class QStringList;

namespace cx
{
}

namespace itk
{
class EventObject;
}

namespace cx
{
typedef std::vector<double> DoubleVector;
typedef boost::shared_ptr<DoubleVector> DoubleVectorPtr;
typedef std::vector<Transform3DPtr> Transform3DVector;
typedef boost::shared_ptr<Transform3DVector> Transform3DVectorPtr;
typedef boost::shared_ptr<class IgstkTool> IgstkToolPtr;

/**
 * \file
 * \addtogroup cx_service_tracking
 * @{
 */

typedef boost::shared_ptr<class cxProbe> cxProbePtr;

/**
 * \brief Class representing the tools a navigation system can recognize.
 * \ingroup cx_service_tracking
 *
 * \date Nov 6, 2008
 * \author Janne Beate Bakeng, SINTEF
 */
class cxTool: public ToolImpl
{
	Q_OBJECT

public:

	/* //only used for documentation purposes
	 TOOL_INVALID_REQUEST,             ///< internal state machine didn't accept the request
	 TOOL_HW_CONFIGURED,               ///< hardware accepted tool as configured
	 TOOL_ATTACHED_TO_TRACKER,         ///< tool accepted by hardware as attached
	 TOOL_VISIBLE,                     ///< tool visible to hardware
	 TOOL_TRACKED,                     ///< tool tracked by hardware
	 TOOL_COORDINATESYSTEM_TRANSFORM,  ///< transform received
	 TOOL_NDI_PORT_NUMBER,             ///< hardware responds to NDI port number
	 TOOL_NDI_SROM_FILENAME,           ///< hardware responds to NDI SROM filename
	 TOOL_NDI_PART_NUMBER,             ///< hardware responds to NDI part number
	 TOOL_AURORA_CHANNEL_NUMBER        ///< hardware responds to Aurora channel number
	 */

	cxTool(TrackingServicePtr manager, IgstkToolPtr igstkTool);
	virtual ~cxTool();

	virtual std::set<Type> getTypes() const;

	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual bool getVisible() const;
	virtual bool isInitialized() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const { return 0; }
	virtual ProbeDefinition getProbeSector() const;
	virtual ProbePtr getProbe() const;
	virtual double getTimestamp() const { return 0; } //	TODO
	virtual double getTooltipOffset() const; ///< get a virtual offset extending from the tool tip.
	virtual void setTooltipOffset(double val); ///< set a virtual offset extending from the tool tip.

	virtual bool isCalibrated() const; ///< true if calibration is different from identity
	virtual Transform3D getCalibration_sMt() const; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
	virtual void setCalibration_sMt(Transform3D calibration); ///< requests to use the calibration and replaces the tools calibration file
	QString getCalibrationFileName() const; ///< returns the path to the tools calibration file

	TRACKING_SYSTEM getTrackerType(); ///< the type of tracker this tool belongs to

	virtual std::map<int, Vector3D> getReferencePoints() const; ///< Get the optional reference points from this tool
	virtual bool hasReferencePointWithId(int id);

	bool isValid() const; ///< whether this tool is constructed correctly or not

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

signals:
	void attachedToTracker(bool);

private slots:
	void toolTransformAndTimestampSlot(Transform3D matrix, double timestamp); ///< timestamp is in milliseconds
	void calculateTpsSlot();
	void toolVisibleSlot(bool);

private:
	void createPolyData(); ///< creates the polydata either from file or a vtkConeSource

	void printInternalStructure(); ///< FOR DEBUGGING

	IgstkToolPtr mTool;

	vtkPolyDataPtr mPolyData; ///< the polydata used to represent the tool graphically
	bool mValid; ///< whether this tool is constructed correctly or not
	bool mConfigured; ///< whether or not the tool is properly configured
	bool mTracked; ///< whether the tool is being tracked or not
	ProbePtr mProbe;
	QTimer mTpsTimer;
};
typedef boost::shared_ptr<cxTool> cxToolPtr;

/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLUSINGIGSTK_H_ */
