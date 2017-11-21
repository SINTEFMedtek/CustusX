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

#ifndef CXTOOLUSINGIGSTK_H_
#define CXTOOLUSINGIGSTK_H_

#include "org_custusx_core_tracking_system_igstk_Export.h"

#include "cxToolImpl.h"

#include <limits.h>
#include <QTimer>
#include <boost/shared_ptr.hpp>
#include "cxTransform3D.h"
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"

class QStringList;

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
 * \addtogroup org_custusx_core_tracking_igstk
 * @{
 */

typedef boost::shared_ptr<class ProbeImpl> ProbeImplPtr;

/**
 * \brief Class representing the tools a navigation system can recognize.
 * \ingroup org_custusx_core_tracking_igstk
 *
 * \date Nov 6, 2008
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_tracking_system_igstk_EXPORT ToolUsingIGSTK : public ToolImpl
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

	ToolUsingIGSTK(IgstkToolPtr igstkTool);
	virtual ~ToolUsingIGSTK();

	virtual bool getVisible() const;
	virtual bool isInitialized() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const { return 0; }
	virtual ProbePtr getProbe() const;
	virtual double getTimestamp() const { return mTimestamp; } //	TODO
	virtual double getTooltipOffset() const; ///< get a virtual offset extending from the tool tip.
	virtual void setTooltipOffset(double val); ///< set a virtual offset extending from the tool tip.

	virtual bool isCalibrated() const; ///< true if calibration is different from identity
	virtual Transform3D getCalibration_sMt() const; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
	virtual void setCalibration_sMt(Transform3D calibration); ///< requests to use the calibration and replaces the tools calibration file
	QString getCalibrationFileName() const; ///< returns the path to the tools calibration file

	TRACKING_SYSTEM getTrackerType(); ///< the type of tracker this tool belongs to

	bool isValid() const; ///< whether this tool is constructed correctly or not

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

    virtual void addXml(QDomNode& dataNode);
    virtual void parseXml(QDomNode& dataNode);

signals:
	void attachedToTracker(bool);

protected:
	virtual ToolFileParser::ToolInternalStructurePtr getToolFileToolStructure() const;
private slots:
	void toolTransformAndTimestampSlot(Transform3D matrix, double timestamp, ToolPositionMetadata metadata); ///< timestamp is in milliseconds
	void calculateTpsSlot();
	void toolVisibleSlot(bool);

private:
	void printInternalStructure(); ///< FOR DEBUGGING

	IgstkToolPtr mTool;

	bool mValid; ///< whether this tool is constructed correctly or not
	bool mConfigured; ///< whether or not the tool is properly configured
	bool mTracked; ///< whether the tool is being tracked or not
	ProbePtr mProbe;
	QTimer mTpsTimer;
	double mTimestamp;
};
typedef boost::shared_ptr<ToolUsingIGSTK> ToolUsingIGSTKPtr;

/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLUSINGIGSTK_H_ */
