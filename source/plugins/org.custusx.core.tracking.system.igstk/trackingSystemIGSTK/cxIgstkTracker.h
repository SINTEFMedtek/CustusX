/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIGSTKTRACKER_H_
#define CXIGSTKTRACKER_H_

#include "org_custusx_core_tracking_system_igstk_Export.h"

#include <QObject>

#include <map>
#include <boost/shared_ptr.hpp>
#include <igstkLogger.h>
#include <igstkTracker.h>
#include <igstkPolarisTracker.h>
#include <igstkAuroraTracker.h>
#include <itkStdStreamLogOutput.h>
#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif
#include "cxTracker.h"
#include "cxTool.h"
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxToolFileParser.h"

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
 * \brief Class representing the navigation system.
 * \ingroup org_custusx_core_tracking_igstk
 *
 * \date Nov 7, 2008
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_tracking_system_igstk_EXPORT IgstkTracker: public Tracker
{
Q_OBJECT
public:

#ifdef WIN32
	/** The type of serial communication used on a windows platform.*/
	typedef igstk::SerialCommunicationForWindows CommunicationType;
#else
	/** The type of serial communication used on Mac and Linux.*/
	typedef igstk::SerialCommunicationForPosix CommunicationType;
#endif
	typedef igstk::Tracker TrackerType;
	typedef igstk::PolarisTracker PolarisTrackerType;
	typedef igstk::AuroraTracker AuroraTrackerType;

	/*only used for documentation purposes
	enum TRACKING_SYSTEM
	{
		tsNONE,             ///< Not specified
		tsPOLARIS,          ///< NDIs Polaris tracker
		tsPOLARIS_SPECTRA,  ///< NDIs Polaris Spectra tracker
		tsPOLARIS_VICRA,    ///< NDIs Polaris Vicra tracker
		tsPOLARIS_CLASSIC,  ///< NDIs Polaris Classic tracker
		tsAURORA,           ///< NDIs Aurora tracker
		tsMICRON,           ///< Claron Technologys Micron tracker
		tsCOUNT
	};*/

	/*only used for documentation purposes
	 TRACKER_INVALID_REQUEST,                    ///< internal state machine didn't accept the request
	 TRACKER_OPEN,                               ///< hardware accepted the tracker as open
	 TRACKER_INITIALIZED,                        ///< hardware accepted the tracker as initialized
	 TRACKER_TRACKING,                           ///< tracker is tracking
	 TRACKER_UPDATE_STATUS,                      ///< tracker got an update event
	 TRACKER_TOOL_TRANSFORM_UPDATED,             ///< a tool attached to the tracker got a new transform
	 TRACKER_COMMUNICATION_COMPLETE,             ///< communication port completed a task successfully
	 TRACKER_COMMUNICATION_INPUT_OUTPUT_ERROR,   ///< communication port failed at completing a task
	 TRACKER_COMMUNICATION_INPUT_OUTPUT_TIMEOUT, ///< communication port timed out
	 TRACKER_COMMUNICATION_OPEN_PORT_ERROR       ///< communication port tried to open or close
	 */

    IgstkTracker(ToolFileParser::TrackerInternalStructure internalStructure);
	~IgstkTracker();

	TRACKING_SYSTEM getType() const; ///< returns the trackers type
	QString getName() const; ///< get the trackers name
	QString getUid() const; ///< get the tracker unique id
	TrackerType* getPointer() const; ///< return a pointer to the internal tracker base
	void open(); ///< open the tracker for communication
	void close(); ///< close the
	void attachTools(std::map<QString, IgstkToolPtr> tools); ///< attach a list of tools to the tracker hw
	void detachTools(std::map<QString, IgstkToolPtr> tools); ///< detach the list of tools from the tracker hw
	void startTracking(); ///< start tracking
	void stopTracking(); ///< stop tracking

	bool isValid() const; ///< whether this tracker is constructed correctly or not
	bool isOpen() const;
	bool isInitialized() const;
	bool isTracking() const;

signals:
	void initialized(bool);
	void open(bool);
	void tracking(bool);
	void error();

protected:
	typedef itk::ReceptorMemberCommand<IgstkTracker> ObserverType;

	void trackerTransformCallback(const itk::EventObject &eventVar); ///< callback receiving events from the observer
	void addLogging(); ///< adds logging to the internal igstk components

	void internalOpen(bool value);
	void internalInitialized(bool value);
	void internalTracking(bool value);
	void internalError(bool value);

	void shutdown(); ///< shuts down the tracker, made to be used when an unrecoverable error occures

    ToolFileParser::TrackerInternalStructure mInternalStructure; ///< the trackers type
	bool mValid; ///< whether this tracker is constructed correctly or not
	QString mUid; ///< the trackers unique id
	QString mName; ///< the trackers name
	TrackerType* mTracker; ///< pointer to the base class of the internal igstk tracker

	PolarisTrackerType::Pointer mTempPolarisTracker; ///< pointer to a temp polaris tracker
	AuroraTrackerType::Pointer mTempAuroraTracker; ///< pointer to a temp aurora tracker

	CommunicationType::Pointer mCommunication; ///< pointer to the serial communication used to communicate with the NDI trackers
	ObserverType::Pointer mTrackerObserver; ///< observer listening for igstk events
	igstk::Logger::Pointer mTrackerLogger; ///< logging the internal igstk behavior
	itk::StdStreamLogOutput::Pointer mTrackerLogOutput; ///< output to write the log to

	bool mOpen; ///< whether or not the tracker is open
	bool mInitialized; ///< whether or not the tracker is initialized
	bool mTracking; ///< whether or not the tracker is tracking

private:
	IgstkTracker() {} ///< do not use this one
};
typedef boost::shared_ptr<IgstkTracker> TrackerPtr;
typedef boost::weak_ptr<IgstkTracker> TrackerWeakPtr;

/**
 * @}
 */
} //namespace cx

#endif /* CXIGSTKTRACKER_H_ */
