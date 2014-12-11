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

#include <QDir>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxSettings.h"
#include "cxTransformFile.h"

namespace cx
{

Transform3D IgstkTool::InternalStructure::getCalibrationAsSSC() const
{
	vtkMatrix4x4Ptr M = vtkMatrix4x4Ptr::New();
	mCalibration.ExportTransform(*(M.GetPointer()));
	Transform3D sMt = Transform3D::fromVtkMatrix(M);
	return sMt;
}

void IgstkTool::InternalStructure::setCalibration(const Transform3D& cal)
{
	mCalibration.ImportTransform(*cal.getVtkMatrix());
}

void IgstkTool::InternalStructure::saveCalibrationToFile()
{
	QString filename = mCalibrationFilename;
//	QFile calibrationFile;
	if (!filename.isEmpty() && QFile::exists(filename))
	{
		//Calibration file exists, overwrite
//		calibrationFile.setFileName(mCalibrationFilename);
	}
	else
	{
		//Make a new file, use rom file name as base name
		filename = mSROMFilename.remove(".rom", Qt::CaseInsensitive);
		filename.append(".cal");
//		calibrationFile.setFileName(calibrationFileName);
	}

	TransformFile file(filename);
	file.write(this->getCalibrationAsSSC());
//
////  Transform3D sMt;
////  vtkMatrix4x4Ptr M = vtkMatrix4x4Ptr::New();
////  mCalibration.ExportTransform(*(M.GetPointer()));
////  Transform3D sMt = Transform3D::fromVtkMatrix(M);
//	Transform3D sMt = this->getCalibrationAsSSC();
//
//	if (!calibrationFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//	{
//		reportError("Could not open " + mUid + "s calibrationfile: " + calibrationFile.fileName());
//		return;
//	}
//
//	QTextStream streamer(&calibrationFile);
//	streamer << qstring_cast(sMt);
//	streamer << endl;
//
//	calibrationFile.close();

	report("Replaced calibration in " + filename);
}


bool IgstkTool::InternalStructure::verify()
{
	bool retval = true;
	QString verificationError("Internal verification of tool " + mUid + " failed! REASON: ");
	if (!mIsPointer && !mIsReference && !mIsProbe)
	{
//    reportError(verificationError+" Tag <tool>::<type> is invalid ["+qstring_cast(mType)+"]. Valid types: [pointer, usprobe, reference]");
		reportError(
						verificationError
										+ " Tag <tool>::<type> is invalid, must be one one of pointer/probe/reference ");
		retval = false;
	}
	if (mUid.isEmpty())
	{
		reportError(verificationError + " Tag <tool>::<uid> is empty. Give tool a unique id.");
		retval = false;
	}
	if (mTrackerType == tsNONE)
	{
		reportError(
						verificationError + " Tag <sensor>::<type> is invalid ["
										+ qstring_cast(mTrackerType)
										+ "]. Valid types: [polaris, spectra, vicra, aurora, micron (NOT SUPPORTED YET)]");
		retval = false;
	}
	if ((mTrackerType == tsAURORA) && (mPortNumber >= 4))
	{
		reportError(
						verificationError + " Tag <sensor>::<portnumber> is invalid ["
										+ qstring_cast(mPortNumber)
										+ "]. Valid numbers: [0, 1, 2, 3]");
		retval = false;
	}
	if ((mTrackerType == tsAURORA) && (mChannelNumber >= 1))
	{
		reportError(
						verificationError + " Tag <sensor>::<channelnumber> is invalid ["
										+ qstring_cast(mChannelNumber) + "]. Valid numbers: [0, 1]");
		retval = false;
	}
	QDir dir;
	if (!mSROMFilename.isEmpty() && !dir.exists(mSROMFilename))
	{
		reportError(
						verificationError + " Tag <sensor>::<rom_file> is invalid [" + mSROMFilename
										+ "]. Valid path: relative path to existing rom file.");
		retval = false;
	}
	if (!mCalibrationFilename.isEmpty() && !dir.exists(mCalibrationFilename))
	{
		reportError(
						verificationError + " Tag <calibration>::<cal_file> is invalid ["
										+ mCalibrationFilename
										+ "]. Valid path: relative path to existing calibration file.");
		retval = false;
	}
	if (!mTransformSaveFileName.isEmpty() && !dir.exists(mTransformSaveFileName))
	{
		reportError(verificationError + " Logging folder is invalid. Contact programmer! :)");
		retval = false;
	}
	if (!mLoggingFolderName.isEmpty() && !dir.exists(mLoggingFolderName))
	{
		reportError(verificationError + " Logging folder is invalid. Contact programmer! :)");
		retval = false;
	}

	return retval;
}

void IgstkTool::updateCalibration(const Transform3D& cal)
{
	//apply the calibration
	mInternalStructure.mCalibration.ImportTransform(*cal.getVtkMatrix());
	this->setCalibrationTransform(mInternalStructure.mCalibration);

	Transform3D sMt = mInternalStructure.getCalibrationAsSSC();
	report("Set " + mInternalStructure.mName + "s calibration to \n" + qstring_cast(sMt));

	//write to file
	mInternalStructure.saveCalibrationToFile();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

IgstkTool::IgstkTool(IgstkTool::InternalStructure internalStructure) :
				mToolObserver(itk::ReceptorMemberCommand<IgstkTool>::New()), mValid(false), mVisible(false), mAttachedToTracker(
								false)
{
	qRegisterMetaType<Transform3D>("Transform3D");

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

IgstkTool::InternalStructure IgstkTool::getInternalStructure()
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

//Tool::Type IgstkTool::getType() const
//{
//  return mInternalStructure.mType;
//}

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

void IgstkTool::toolTransformCallback(const itk::EventObject &event)
{
	if (igstk::CoordinateSystemTransformToEvent().CheckEvent(&event))
	{
		const igstk::CoordinateSystemTransformToEvent *transformEvent;
		transformEvent = dynamic_cast<const igstk::CoordinateSystemTransformToEvent*>(&event);
		if (!transformEvent)
			return;

		igstk::CoordinateSystemTransformToResult result = transformEvent->Get();
		igstk::Transform transform = result.GetTransform();
		if (transform.IsIdentity())
			return;
		if (!transform.IsValidNow())
		{
			//What to do? this happens alot, dunno why. Ignore? Seems to work ok.
			//CA20100901: Probable cause: we work on the main (render) thread. This causes several hundred ms lag. Move IGSTK+Toolmanager internals to separate thread.
			//TODO need to find out why this happens, we get duplicate transforms, it seems, this is not good
			//return;
		}
		if (!mVisible)
			return; // quickfix replacement for IsValidNow()

		const igstk::CoordinateSystem* destination = result.GetDestination();
		IgstkToolPtr strongReference = mReferenceTool.lock();

		if (strongReference) //if we are tracking with a reftool it must be visible
		{
			if (!strongReference->getPointer()->IsCoordinateSystem(destination))
				return;
		}
		else //if we dont have a reftool we use the tracker as patientref
		{
			TrackerPtr tracker = mTracker.lock();
			if (!tracker || !tracker->getPointer()->IsCoordinateSystem(destination))
				return;
		}

		vtkMatrix4x4Ptr vtkMatrix = vtkMatrix4x4Ptr::New();
		transform.ExportTransform(*vtkMatrix.GetPointer());

		const Transform3D prMt(vtkMatrix.GetPointer()); //prMt, transform from tool to patientref
		double timestamp = transform.GetStartTime();

		emit toolTransformAndTimestamp(prMt, timestamp);
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
		tempPolarisTool->SetCalibrationTransform(mInternalStructure.mCalibration);
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
		tempAuroraTool->SetCalibrationTransform(mInternalStructure.mCalibration);
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

void IgstkTool::setCalibrationTransform(igstk::Transform calibration)
{
	mInternalStructure.mCalibration = calibration;
	mTool->SetCalibrationTransform(calibration);
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
	std::cout << "mCalibration: " << std::endl;
	mInternalStructure.mCalibration.Print(std::cout, itk::Indent());
	std::cout << "mCalibrationFilename: " << mInternalStructure.mCalibrationFilename << std::endl;
	std::cout << "mGraphicsFileName: " << mInternalStructure.mGraphicsFileName << std::endl;
	std::cout << "mTransformSaveFileName: " << mInternalStructure.mTransformSaveFileName << std::endl;
	std::cout << "mLoggingFolderName: " << mInternalStructure.mLoggingFolderName << std::endl;
	std::cout << "------------------------------------------------------------------" << std::endl;
}

} //namespace cx
