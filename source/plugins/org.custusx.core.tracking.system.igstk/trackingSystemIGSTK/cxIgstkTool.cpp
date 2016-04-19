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

#include "cxIgstkTool.h"

#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include <vtkTransform.h>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxSettings.h"
#include "cxTransformFile.h"
#include "cxIgstkToolManager.h"

namespace cx
{

igstk::Transform IgstkTool::toIgstkTransform(Transform3D transform)
{
    igstk::Transform retval;
    retval.ImportTransform(*transform.getVtkMatrix());
    return retval;
}

Transform3D IgstkTool::toTransform3D(igstk::Transform transform)
{
    Transform3D retval;
    vtkMatrix4x4Ptr matrix = vtkMatrix4x4Ptr::New();
    transform.ExportTransform(*(matrix.GetPointer()));
    retval = Transform3D::fromVtkMatrix(matrix);
    return retval;
}

void IgstkTool::updateCalibration(const Transform3D& cal)
{
	//apply the calibration
    mInternalStructure.mCalibration = cal;
    this->setCalibrationTransform(mInternalStructure.mCalibration);

	Transform3D sMt = mInternalStructure.getCalibrationAsSSC();
	report("Set " + mInternalStructure.mName + "s calibration to \n" + qstring_cast(sMt));

	//write to file
	mInternalStructure.saveCalibrationToFile();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

IgstkTool::IgstkTool(ToolFileParser::ToolInternalStructure internalStructure) :
				mToolObserver(itk::ReceptorMemberCommand<IgstkTool>::New()), mValid(false), mVisible(false), mAttachedToTracker(
								false)
{
	qRegisterMetaType<Transform3D>("Transform3D");
	qRegisterMetaType<ToolPositionMetadata>("ToolPositionMetadata");

	mLatestEmittedTimestamp = 0;

	mInternalStructure = internalStructure;

	mToolObserver->SetCallbackFunction(this, &IgstkTool::toolTransformCallback);

	if (mInternalStructure.verify())
	{
		mTool = this->buildInternalTool();
		this->addLogging();
		mValid = true;
	}
	else
	{
		reportError(mInternalStructure.mUid + " was created with invalid internal structure.");
		mValid = false;
	}
}

IgstkTool::~IgstkTool()
{
}

ToolFileParser::ToolInternalStructure IgstkTool::getInternalStructure()
{
	return mInternalStructure;
}

QString IgstkTool::getUid()
{
	return mInternalStructure.mUid;
}

igstk::TrackerTool::Pointer IgstkTool::getPointer() const
{
	return mTool;
}

TRACKING_SYSTEM IgstkTool::getTrackerType()
{
	return mInternalStructure.mTrackerType;
}

bool IgstkTool::isValid() const
{
	return mValid;
}

bool IgstkTool::isInitialized() const
{
	return mAttachedToTracker;
}

bool IgstkTool::isTracked() const
{
	return mTracked;
}

bool IgstkTool::isVisible() const
{
	return mVisible;
}

void IgstkTool::setReference(IgstkToolPtr refTool)
{
	mReferenceTool = refTool;
}

void IgstkTool::setTracker(TrackerPtr tracker)
{
	mTracker = tracker;
}

igstk::NDITracker::TrackingSampleInfo IgstkTool::getSampleInfo()
{
	igstk::NDITracker::TrackingSampleInfo retval;
	retval.m_FrameNumber = 0;

	TrackerPtr tracker = mTracker.lock();
	if (!tracker)
		return retval;
	igstk::NDITracker* ndiTracker = dynamic_cast<igstk::NDITracker*>(tracker->getPointer());
	if (!ndiTracker)
		return retval;

	// hacking into the IGSTK internals...
	std::map<std::string,igstk::NDITracker::TrackingSampleInfo> info = ndiTracker->GetTrackingSampleInfo();
	return info[mTool->GetTrackerToolIdentifier()];
}

bool IgstkTool::validReferenceForResult(igstk::CoordinateSystemTransformToResult result)
{
	const igstk::CoordinateSystem* destination = result.GetDestination();
	IgstkToolPtr refTool = mReferenceTool.lock();

	if (refTool) //if we are tracking with a reftool it must be visible
	{
		if (!refTool->getPointer()->IsCoordinateSystem(destination))
			return false;
	}
	else //if we dont have a reftool we use the tracker as patientref
	{
		TrackerPtr tracker = mTracker.lock();
		if (!tracker || !tracker->getPointer()->IsCoordinateSystem(destination))
			return false;
	}

	return true;
}

Transform3D IgstkTool::igstk2Transform3D(const igstk::Transform& input) const
{
	vtkMatrix4x4Ptr vtkMatrix = vtkMatrix4x4Ptr::New();
	input.ExportTransform(*vtkMatrix.GetPointer());
	return Transform3D(vtkMatrix.GetPointer());
}

void IgstkTool::processReceivedTransformResult(igstk::CoordinateSystemTransformToResult result)
{
	if (!this->validReferenceForResult(result))
		return;

//	if (!mVisible)
//		CX_LOG_CHANNEL_DEBUG("CA") << "*********************************************   not visible" << this->mInternalStructure.mName;

//	if (!mVisible)
//		return;

	igstk::NDITracker::TrackingSampleInfo sampleInfo = this->getSampleInfo();

	// ignore duplicate positions
	if (similar(mLatestEmittedTimestamp, sampleInfo.m_TimeStamp.GetStartTime(),1.0E-3))
	{
//		CX_LOG_CHANNEL_DEBUG("CA") << "*********************************************   duplicate transform";
		return;
	}
	mLatestEmittedTimestamp = sampleInfo.m_TimeStamp.GetStartTime();

	QDomDocument doc;
	QDomElement root = doc.createElement("info");
	doc.appendChild(root);
	sampleInfo2xml(sampleInfo, root);
	ToolPositionMetadata metadata;
	metadata.mData = doc.toString();

	igstk::Transform transform = result.GetTransform();
//		if (transform.IsIdentity())
//			return; // we want to update metadata for identity tools too.
	Transform3D prMt = igstk2Transform3D(transform);
	double timestamp = transform.GetStartTime();

	emit toolTransformAndTimestamp(prMt, timestamp, metadata);
}

void IgstkTool::toolTransformCallback(const itk::EventObject &event)
{
	if (igstk::CoordinateSystemTransformToEvent().CheckEvent(&event))
	{
		const igstk::CoordinateSystemTransformToEvent *transformEvent;
		transformEvent = dynamic_cast<const igstk::CoordinateSystemTransformToEvent*>(&event);
		if (!transformEvent)
			return;		
		this->processReceivedTransformResult(transformEvent->Get());
	}
	//Successes
	else if (igstk::TrackerToolConfigurationEvent().CheckEvent(&event))
	{
		//this->internalConfigured(true);
		report(QString("Configured [%1] with the tracking system").arg(mInternalStructure.mUid));
	}
	else if (igstk::TrackerToolAttachmentToTrackerEvent().CheckEvent(&event))
	{
		this->internalAttachedToTracker(true);
	}
	else if (igstk::TrackerToolDetachmentFromTrackerEvent().CheckEvent(&event))
	{
		this->internalAttachedToTracker(false);
	}
	else if (igstk::TrackerToolMadeTransitionToTrackedStateEvent().CheckEvent(&event))
	{
		this->internalVisible(true);
		//report(mInternalStructure.mUid+" is visible."); //SPAM
	}
	else if (igstk::TrackerToolNotAvailableToBeTrackedEvent().CheckEvent(&event))
	{
		this->internalVisible(false);
		//report(mInternalStructure.mUid+" is hidden."); //SPAM
	}
	else if (igstk::ToolTrackingStartedEvent().CheckEvent(&event))
	{
		this->internalTracked(true);
		report(mInternalStructure.mUid + " is tracked.");
	}
	else if (igstk::ToolTrackingStoppedEvent().CheckEvent(&event))
	{
		this->internalTracked(false);
		report(mInternalStructure.mUid + " is not tracked anymore.");
	}
	//Failures
	else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
	{
		reportWarning(
						mInternalStructure.mUid
										+ " received an invalid request.  This means that the internal igstk trackertool did not accept the request. Do not know which request.");
	}
	else if (igstk::TrackerToolConfigurationErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " could not configure with the tracking system.");
	}
	else if (igstk::InvalidRequestToAttachTrackerToolErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " could not request to attach to tracker.");
	}
	else if (igstk::InvalidRequestToDetachTrackerToolErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " could not request to detach from tracker.");
	}
	else if (igstk::TrackerToolAttachmentToTrackerErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " could not attach to tracker.");
	}
	else if (igstk::TrackerToolDetachmentFromTrackerErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " could not detach from tracker.");
	}
	//Polaris specific failures
	else if (igstk::InvalidPolarisPortNumberErrorEvent().CheckEvent(&event))
	{
		reportError(
						mInternalStructure.mUid + " sendt invalid Polaris port number: "
										+ qstring_cast(mInternalStructure.mPortNumber) + ".");
	}
	else if (igstk::InvalidPolarisSROMFilenameErrorEvent().CheckEvent(&event))
	{
		reportError(
						mInternalStructure.mUid + " sendt invalid ROM file: " + mInternalStructure.mSROMFilename);
	}
	else if (igstk::InvalidPolarisPartNumberErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " has an invalid part number.");
	}
	//Aurora specific failures
	else if (igstk::InvalidAuroraPortNumberErrorEvent().CheckEvent(&event))
	{
		reportError(
						mInternalStructure.mUid + " has an invalid port number: "
										+ qstring_cast(mInternalStructure.mPortNumber) + ".");
	}
	else if (igstk::InvalidAuroraSROMFilenameErrorEvent().CheckEvent(&event))
	{
		reportError(
						mInternalStructure.mUid + " sendt invalid ROM file: " + mInternalStructure.mSROMFilename);
	}
	else if (igstk::InvalidAuroraPartNumberErrorEvent().CheckEvent(&event))
	{
		reportError(mInternalStructure.mUid + " has an invalid part number.");
	}
	else if (igstk::InvalidAuroraChannelNumberErrorEvent().CheckEvent(&event))
	{
		reportError(
						mInternalStructure.mUid + " has an invalid channel number:"
										+ qstring_cast(mInternalStructure.mChannelNumber) + ".");
	}
}

igstk::TrackerTool::Pointer IgstkTool::buildInternalTool()
{
	igstk::TrackerTool::Pointer tool;

	igstk::PolarisTrackerTool::Pointer tempPolarisTool;
	igstk::AuroraTrackerTool::Pointer tempAuroraTool;

    igstk::Transform calibration = toIgstkTransform(mInternalStructure.mCalibration);
	switch (mInternalStructure.mTrackerType)
	{
	case tsNONE:
		break;
	case tsPOLARIS_SPECTRA:
	case tsPOLARIS_VICRA:
	case tsPOLARIS:
		tempPolarisTool = igstk::PolarisTrackerTool::New();
		tempPolarisTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
		if (!mInternalStructure.mWireless) //we only support wireless atm
			return tool = tempPolarisTool.GetPointer();
		tempPolarisTool->RequestSelectWirelessTrackerTool();
		tempPolarisTool->RequestSetSROMFileName(string_cast(mInternalStructure.mSROMFilename));
		tempPolarisTool->RequestConfigure();
        tempPolarisTool->SetCalibrationTransform(calibration);
		tool = tempPolarisTool;
		break;
	case tsAURORA:
		tempAuroraTool = igstk::AuroraTrackerTool::New();
		tempAuroraTool->AddObserver(igstk::IGSTKEvent(), mToolObserver);
		if (mInternalStructure.m5DOF)
		{
			tempAuroraTool->RequestSelect5DOFTrackerTool();
			tempAuroraTool->RequestSetPortNumber(mInternalStructure.mPortNumber);
			tempAuroraTool->RequestSetChannelNumber(mInternalStructure.mChannelNumber);
		}
		else
		{
			tempAuroraTool->RequestSelect6DOFTrackerTool();
			tempAuroraTool->RequestSetPortNumber(mInternalStructure.mPortNumber);
		}
		tempAuroraTool->RequestConfigure();
        tempAuroraTool->SetCalibrationTransform(calibration);
		tool = tempAuroraTool;
		break;
	case tsMICRON:
		//TODO: implement
		break;
	default:
		break;
	}
	return tool;
}

void IgstkTool::setCalibrationTransform(Transform3D calibration)
{
	mInternalStructure.mCalibration = calibration;
    igstk::Transform transform = toIgstkTransform(mInternalStructure.mCalibration);
    mTool->SetCalibrationTransform(transform);
}

void IgstkTool::internalAttachedToTracker(bool value)
{
	if (mAttachedToTracker == value)
		return;
	mAttachedToTracker = value;
	report(
					mInternalStructure.mUid + " is " + (value ? "at" : "de") + "tached " + (value ? "to" : "from")
									+ " the tracker.");
	emit attachedToTracker(mAttachedToTracker);
}

void IgstkTool::internalTracked(bool value)
{
	if (mTracked == value)
		return;
	mTracked = value;

	if (!mTracked && mVisible)
		this->internalVisible(false); //Make sure tool is invisible when not tracked

	emit tracked(mTracked);
}

void IgstkTool::internalVisible(bool value)
{
	if (mVisible == value)
		return;
	mVisible = value;
	emit toolVisible(mVisible);
}

void IgstkTool::addLogging()
{
	bool logging = settings()->value("IGSTKDebugLogging", true).toBool();
	if (logging)
	{
		std::ofstream* loggerFile = new std::ofstream();
		QString logFile = mInternalStructure.mLoggingFolderName + "Tool_" + mInternalStructure.mName + "_Logging.txt";
		loggerFile->open(cstring_cast(logFile));
		mLogger = igstk::Logger::New();
		mLogOutput = itk::StdStreamLogOutput::New();
		mLogOutput->SetStream(*loggerFile);
		mLogger->AddLogOutput(mLogOutput);
		mLogger->SetPriorityLevel(itk::Logger::DEBUG);

		mTool->SetLogger(mLogger);
	}
}

void IgstkTool::printInternalStructure()
{
	std::cout << "------------------------------------------------------------------" << std::endl;
	std::cout << "mIsProbe: " << mInternalStructure.mIsProbe << std::endl;
	std::cout << "mIsReference: " << mInternalStructure.mIsReference << std::endl;
	std::cout << "mIsPointer: " << mInternalStructure.mIsPointer << std::endl;
	std::cout << "mName: " << mInternalStructure.mName << std::endl;
	std::cout << "mUid: " << mInternalStructure.mUid << std::endl;
	std::cout << "mTrackerType: " << mInternalStructure.mTrackerType << std::endl;
	std::cout << "mSROMFilename: " << mInternalStructure.mSROMFilename << std::endl;
	std::cout << "mPortNumber: " << mInternalStructure.mPortNumber << std::endl;
	std::cout << "mChannelNumber: " << mInternalStructure.mChannelNumber << std::endl;
	std::cout << "mReferencePoints: " << string_cast(mInternalStructure.mReferencePoints.size()) << std::endl;
	std::cout << "mWireless: " << mInternalStructure.mWireless << std::endl;
	std::cout << "m5DOF: " << mInternalStructure.m5DOF << std::endl;
    std::cout << "mCalibration: " << mInternalStructure.mCalibration << std::endl;
    //mInternalStructure.mCalibration.Print(std::cout, itk::Indent());"
	std::cout << "mCalibrationFilename: " << mInternalStructure.mCalibrationFilename << std::endl;
	std::cout << "mGraphicsFileName: " << mInternalStructure.mGraphicsFileName << std::endl;
	std::cout << "mTransformSaveFileName: " << mInternalStructure.mTransformSaveFileName << std::endl;
	std::cout << "mLoggingFolderName: " << mInternalStructure.mLoggingFolderName << std::endl;
	std::cout << "------------------------------------------------------------------" << std::endl;
}

} //namespace cx
