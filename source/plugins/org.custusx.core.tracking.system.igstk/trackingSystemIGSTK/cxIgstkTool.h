/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIGSTKTOOL_H_
#define CXIGSTKTOOL_H_

#include "org_custusx_core_tracking_system_igstk_Export.h"

#include <QObject>
#include "cxToolUsingIGSTK.h"

#include <limits.h>
#include <boost/shared_ptr.hpp>
#include <igstkPolarisTrackerTool.h>
#include <igstkAuroraTrackerTool.h>
#include <igstkTransform.h>
#include <igstkLogger.h>
#include <itkStdStreamLogOutput.h>
#include "cxTransform3D.h"
#include "cxIgstkTracker.h"
#include "vtkForwardDeclarations.h"
#include "igstkNDITracker.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_igstk
 * @{
 */

class IgstkTool;
typedef boost::shared_ptr<IgstkTool> IgstkToolPtr;
typedef boost::weak_ptr<IgstkTool> IgstkToolWeakPtr;

/**
 * \brief  Class for controlling the igstk tracking (hardware) interface.
 * \ingroup org_custusx_core_tracking_igstk
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_system_igstk_EXPORT IgstkTool: public QObject
{
Q_OBJECT

public:
    static igstk::Transform toIgstkTransform(Transform3D transform);
    static Transform3D toTransform3D(igstk::Transform transform);

		IgstkTool(ToolFileParser::ToolInternalStructurePtr internalStructure);
	virtual ~IgstkTool();

		ToolFileParser::ToolInternalStructurePtr getInternalStructure();
	QString getUid();

	igstk::TrackerTool::Pointer getPointer() const; ///< return a pointer to the internal tools base object
	TRACKING_SYSTEM getTrackerType();
//  Tool::Type getType() const;
	bool isReference() const
	{
		return mInternalStructure->mIsReference;
	}

	bool isValid() const; ///< Thread safe, volatile
	bool isInitialized() const; ///< Thread safe, volatile
	bool isTracked() const; ///< Thread safe, volatile
	bool isVisible() const; ///< Thread safe, volatile

	void setReference(IgstkToolPtr);
	void setTracker(TrackerPtr tracker);
    void setCalibrationTransform(Transform3D calibration);
	void updateCalibration(const Transform3D& sMt);

	void printInternalStructure();

signals:
	void attachedToTracker(bool);
	void tracked(bool);
	void toolVisible(bool);
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp, ToolPositionMetadata metadata);

private:
	void toolTransformCallback(const itk::EventObject &event);
	igstk::TrackerTool::Pointer buildInternalTool();
	void determineToolsCalibration();
	void internalAttachedToTracker(bool value);
	void internalTracked(bool value);
	void internalVisible(bool value);
	void addLogging(); ///< adds igstk logging to the internal igstk trackertool
	igstk::NDITracker::TrackingSampleInfo getSampleInfo();
	bool validReferenceForResult(igstk::CoordinateSystemTransformToResult result);

	ToolFileParser::ToolInternalStructurePtr mInternalStructure; ///< the structure that defines the tool characteristics
	igstk::TrackerTool::Pointer mTool; ///< pointer to the base class of the igstk tool
	IgstkToolWeakPtr mReferenceTool; ///< the tool that is used as a reference to the tracking system
	TrackerWeakPtr mTracker; ///< the tracker this tool belongs to
	itk::ReceptorMemberCommand<IgstkTool>::Pointer mToolObserver; ///< observer listening for igstk events
	igstk::Logger::Pointer mLogger; ///< logging the internal igstk behavior
	itk::StdStreamLogOutput::Pointer mLogOutput; ///< output to write the log to

	double mLatestEmittedTimestamp;
	//internal states
	//TODO is volatile enough for thread safety?
	volatile bool mValid; ///< whether or not the tool is valid or not
	volatile bool mVisible; ///< whether or not the tool is visible to the tracking system
	volatile bool mAttachedToTracker; ///< whether the tool is attached to a tracker or not
	volatile bool mTracked; ///< whether the tool is being tracked or not

	Transform3D igstk2Transform3D(const igstk::Transform &input) const;
	void processReceivedTransformResult(igstk::CoordinateSystemTransformToResult result);
};

/**
 * @}
 */
}

#endif /* CXIGSTKTOOL_H_ */
