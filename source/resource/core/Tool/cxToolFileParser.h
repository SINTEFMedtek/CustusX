/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLFILEPARSER_H
#define CXTOOLFILEPARSER_H

#include "cxResourceExport.h"

#include <QDomDocument>
#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxDefinitions.h"
#include <vector>
#include <map>

namespace cx {

/**
 * \brief Class for reading the files defining a CustusX tool
 * \ingroup org_custusx_core_tracking
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT ToolFileParser
{
public:
    /**A trackers internal structure \warning make sure you set all the members to an appropriate value.*/
    struct cxResource_EXPORT TrackerInternalStructure
    {
        TRACKING_SYSTEM mType; ///< the trackers type
        QString mLoggingFolderName; ///< path to where log should be saved
        TrackerInternalStructure() :
                        mType(tsNONE),
                        mLoggingFolderName("")
        {} ///< set default values for the internal structure
    };

    /**\brief A tools internal structure
     * \warning make sure you set all the members to an appropriate value.
     */
    struct cxResource_EXPORT ToolInternalStructure
    {
        Transform3D getCalibrationAsSSC() const;
        void setCalibration(const Transform3D& cal);
        void saveCalibrationToFile();
        bool verify();

        bool mIsReference;
        bool mIsPointer;
        bool mIsProbe;
//    Tool::Type   mType;                  ///< the tools type
        QString mName; ///< the tools name
        QString mUid; ///< the tools unique id
        std::vector<QString> mClinicalApplications; ///< the tools clinical application applications
        TRACKING_SYSTEM mTrackerType; ///< what product the tool belongs to
        QString mSROMFilename; ///< path to the tools SROM file
        unsigned int mPortNumber; ///< the port number the tool is connected to
        unsigned int mChannelNumber; ///< the channel the tool is connected to
        std::map<int, Vector3D> mReferencePoints; ///< optional point on the frame, specifying a known reference point, 0,0,0 is default, in sensor space
        bool mWireless; ///< whether or not the tool is wireless
        bool m5DOF; ///< whether or not the tool have 5 DOF
        //TODO use Transform3D instead
        Transform3D mCalibration; ///< transform read from mCalibrationFilename
        QString mCalibrationFilename; ///< path to the tools calibration file
        QString mGraphicsFileName; ///< path to this tools graphics file
        QString mPictureFileName; ///< path to picture of the tool
        QString mTransformSaveFileName; ///< path to where transforms should be saved
        QString mLoggingFolderName; ///< path to where log should be saved
        QString mInstrumentId; ///< The instruments id
        QString mInstrumentScannerId; ///< The id of the ultrasound scanner if the instrument is a probe
        ToolInternalStructure() :
                        mIsReference(false), mIsPointer(false), mIsProbe(false),
                        //mType(Tool::TOOL_NONE),
                        mName(""), mUid(""),
                        mTrackerType(tsNONE), mSROMFilename(""),
                        mPortNumber(UINT_MAX), mChannelNumber(UINT_MAX),
                        mReferencePoints(), mWireless(true),
												m5DOF(true),
												mCalibration(Transform3D::Identity()),
												mCalibrationFilename(""),
                        mGraphicsFileName(""), mPictureFileName(""),
                        mTransformSaveFileName(""),
                        mLoggingFolderName(""), mInstrumentId(""),
                        mInstrumentScannerId("")
        {}	///< sets up default values for all the members
    };

		typedef boost::shared_ptr<ToolInternalStructure> ToolInternalStructurePtr;

public:
    ToolFileParser(QString absoluteToolFilePath, QString loggingFolder = "");
		virtual ~ToolFileParser();

		virtual ToolInternalStructurePtr getTool();

    //static QString getTemplatesAbsoluteFilePath();

protected:
    QDomNode getToolNode(QString toolAbsoluteFilePath);
    Transform3D readCalibrationFile(QString absoluteFilePath);

    QString mToolFilePath; ///< absolutepath to the tool file
    QString mLoggingFolder; ///< absolutepath to the logging folder

    QDomDocument mToolDoc; ///< the tool xml document
    const QString mToolTag, mToolTypeTag, mToolIdTag, mToolNameTag, mToolDescriptionTag, mToolManufacturerTag,
                    mToolClinicalAppTag, mToolGeoFileTag, mToolPicFileTag, mToolDocFileTag, mToolInstrumentTag,
                    mToolInstrumentTypeTag, mToolInstrumentIdTag, mToolInstrumentNameTag,
                    mToolInstrumentManufacturerTag, mToolInstrumentScannerIdTag, mToolInstrumentDescriptionTag,
                    mToolSensorTag, mToolSensorTypeTag, mToolSensorIdTag, mToolSensorNameTag, mToolSensorWirelessTag,
                    mToolSensorDOFTag, mToolSensorPortnumberTag, mToolSensorChannelnumberTag,
                    mToolSensorReferencePointTag, mToolSensorManufacturerTag, mToolSensorDescriptionTag,
                    mToolSensorRomFileTag, mToolCalibrationTag, mToolCalibrationFileTag;
    ///< names of necessary tags in the tool file

};
}//namespace


#endif // CXTOOLFILEPARSER_H
