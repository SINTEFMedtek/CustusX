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

#ifndef CXIGSTKTOOL_H_
#define CXIGSTKTOOL_H_

#include <QObject>
#include "cxTool.h"

#include <limits.h>
#include <boost/shared_ptr.hpp>
#include <igstkPolarisTrackerTool.h>
#include <igstkAuroraTrackerTool.h>
#include <igstkTransform.h>
#include <igstkLogger.h>
#include <itkStdStreamLogOutput.h>
#include "sscTransform3D.h"
#include "cxIgstkTracker.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceTracking
 * @{
 */

class IgstkTool;
typedef boost::shared_ptr<IgstkTool> IgstkToolPtr;
typedef boost::weak_ptr<IgstkTool> IgstkToolWeakPtr;

/**
 * \brief  Class for controlling the igstk tracking (hardware) interface.
 * \ingroup cxServiceTracking
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkTool: public QObject
{
Q_OBJECT

public:
	/**\brief A tools internal structure
	 * \warning make sure you set all the members to an appropriate value.
	 */
	struct InternalStructure
	{
		ssc::Transform3D getCalibrationAsSSC() const;
		void setCalibration(const ssc::Transform3D& cal);
		void saveCalibrationToFile();

		bool mIsReference;
		bool mIsPointer;
		bool mIsProbe;
//    ssc::Tool::Type   mType;                  ///< the tools type
		QString mName; ///< the tools name
		QString mUid; ///< the tools unique id
		std::vector<ssc::CLINICAL_APPLICATION> mClinicalApplications; ///< the tools clinical application applications
		ssc::TRACKING_SYSTEM mTrackerType; ///< what product the tool belongs to
		QString mSROMFilename; ///< path to the tools SROM file
		unsigned int mPortNumber; ///< the port number the tool is connected to
		unsigned int mChannelNumber; ///< the channel the tool is connected to
		std::map<int, ssc::Vector3D> mReferencePoints; ///< optional point on the frame, specifying a known reference point, 0,0,0 is default, in sensor space
		bool mWireless; ///< whether or not the tool is wireless
		bool m5DOF; ///< whether or not the tool have 5 DOF
		igstk::Transform mCalibration; ///< transform read from mCalibrationFilename
		QString mCalibrationFilename; ///< path to the tools calibration file
		QString mGraphicsFileName; ///< path to this tools graphics file
		QString mPictureFileName; ///< path to picture of the tool
		QString mTransformSaveFileName; ///< path to where transforms should be saved
		QString mLoggingFolderName; ///< path to where log should be saved
		QString mInstrumentId; ///< The instruments id
		QString mInstrumentScannerId; ///< The id of the ultrasound scanner if the instrument is a probe
		InternalStructure() :
						mIsReference(false), mIsPointer(false), mIsProbe(false),
						//mType(ssc::Tool::TOOL_NONE),
						mName(""), mUid(""),
						mTrackerType(ssc::tsNONE), mSROMFilename(""),
						mPortNumber(UINT_MAX), mChannelNumber(UINT_MAX),
						mReferencePoints(), mWireless(true),
						m5DOF(true), mCalibrationFilename(""),
						mGraphicsFileName(""), mPictureFileName(""),
						mTransformSaveFileName(""),
						mLoggingFolderName(""), mInstrumentId(""),
						mInstrumentScannerId("")
		{}	///< sets up default values for all the members
	};

public:
	IgstkTool(InternalStructure internalStructure);
	virtual ~IgstkTool();

	InternalStructure getInternalStructure();
	QString getUid();

	igstk::TrackerTool::Pointer getPointer() const; ///< return a pointer to the internal tools base object
	ssc::TRACKING_SYSTEM getTrackerType();
//  ssc::Tool::Type getType() const;
	bool isReference() const
	{
		return mInternalStructure.mIsReference;
	}

	bool isValid() const; ///< Thread safe, volatile
	bool isInitialized() const; ///< Thread safe, volatile
	bool isTracked() const; ///< Thread safe, volatile
	bool isVisible() const; ///< Thread safe, volatile

	void setReference(IgstkToolPtr);
	void setTracker(TrackerPtr tracker);
	void setCalibrationTransform(igstk::Transform calibration);
	void updateCalibration(const ssc::Transform3D& sMt);

	void printInternalStructure();

	typedef ssc::Transform3D Transform3D;

signals:
	void attachedToTracker(bool);
	void tracked(bool);
	void toolVisible(bool);
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void tps(int); ///< transforms per second

private:
	void toolTransformCallback(const itk::EventObject &event);
	bool verifyInternalStructure();
	igstk::TrackerTool::Pointer buildInternalTool();
	void determineToolsCalibration();
	void internalAttachedToTracker(bool value);
	void internalTracked(bool value);
	void internalVisible(bool value);
	void addLogging(); ///< adds igstk logging to the internal igstk trackertool

	InternalStructure mInternalStructure; ///< the structure that defines the tool characteristics
	igstk::TrackerTool::Pointer mTool; ///< pointer to the base class of the igstk tool
	IgstkToolWeakPtr mReferenceTool; ///< the tool that is used as a reference to the tracking system
	TrackerWeakPtr mTracker; ///< the tracker this tool belongs to
	itk::ReceptorMemberCommand<IgstkTool>::Pointer mToolObserver; ///< observer listening for igstk events
	igstk::Logger::Pointer mLogger; ///< logging the internal igstk behavior
	itk::StdStreamLogOutput::Pointer mLogOutput; ///< output to write the log to

	//internal states
	//TODO is volatile enough for thread safety?
	volatile bool mValid; ///< whether or not the tool is valid or not
	volatile bool mVisible; ///< whether or not the tool is visible to the tracking system
	volatile bool mAttachedToTracker; ///< whether the tool is attached to a tracker or not
	volatile bool mTracked; ///< whether the tool is being tracked or not

};

/**
 * @}
 */
}

#endif /* CXIGSTKTOOL_H_ */
