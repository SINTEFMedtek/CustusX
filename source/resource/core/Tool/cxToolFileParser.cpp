#include "cxToolFileParser.h"

#include <QFile>
#include <QDir>
#include "cxLogger.h"
#include "cxTransformFile.h"

namespace cx {

Transform3D ToolFileParser::ToolInternalStructure::getCalibrationAsSSC() const
{
    //vtkMatrix4x4Ptr M = vtkMatrix4x4Ptr::New();
    //mCalibration.ExportTransform(*(M.GetPointer()));
    //Transform3D sMt = Transform3D::fromVtkMatrix(M);
    //return sMt;
    return mCalibration;
}

void ToolFileParser::ToolInternalStructure::setCalibration(const Transform3D& cal)
{
    mCalibration = cal;
    //mCalibration.ImportTransform(*cal.getVtkMatrix());
}

void ToolFileParser::ToolInternalStructure::saveCalibrationToFile()
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

bool ToolFileParser::ToolInternalStructure::verify()
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

ToolFileParser::ToolFileParser(QString absoluteToolFilePath, QString loggingFolder) :
                mToolFilePath(absoluteToolFilePath), mLoggingFolder(loggingFolder), mToolTag("tool"), mToolTypeTag(
                                "type"), mToolIdTag("id"), mToolNameTag("name"), mToolDescriptionTag("description"), mToolManufacturerTag(
                                "manufacturer"), mToolClinicalAppTag("clinical_app"), mToolGeoFileTag("geo_file"), mToolPicFileTag(
                                "pic_file"), mToolDocFileTag("doc_file"), mToolInstrumentTag("instrument"), mToolInstrumentTypeTag(
                                "type"), mToolInstrumentIdTag("id"), mToolInstrumentNameTag("name"), mToolInstrumentManufacturerTag(
                                "manufacturer"), mToolInstrumentScannerIdTag("scannerid"), mToolInstrumentDescriptionTag(
                                "description"), mToolSensorTag("sensor"), mToolSensorTypeTag("type"), mToolSensorIdTag(
                                "id"), mToolSensorNameTag("name"), mToolSensorWirelessTag("wireless"), mToolSensorDOFTag(
                                "DOF"), mToolSensorPortnumberTag("portnumber"), mToolSensorChannelnumberTag(
                                "channelnumber"), mToolSensorReferencePointTag("reference_point"), mToolSensorManufacturerTag(
                                "manufacturer"), mToolSensorDescriptionTag("description"), mToolSensorRomFileTag(
                                "rom_file"), mToolCalibrationTag("calibration"), mToolCalibrationFileTag("cal_file")
{
}

ToolFileParser::~ToolFileParser()
{
}

ToolFileParser::ToolInternalStructure ToolFileParser::getTool()
{
    ToolInternalStructure retval;

    return retval;
}

QDomNode ToolFileParser::getToolNode(QString toolAbsoluteFilePath)
{
    QDomNode retval;
    QFile toolFile(toolAbsoluteFilePath);
    if (!mToolDoc.setContent(&toolFile))
    {
        reportError("Could not set the xml content of the tool file " + toolAbsoluteFilePath);
        return retval;
    }
    //there can only be one tool defined in every tool.xml-file, that's why we say ...item(0)
    retval = mToolDoc.elementsByTagName(mToolTag).item(0);
    return retval;
}

/*
QString ToolFileParser::getTemplatesAbsoluteFilePath()
{
    QString retval = DataLocations::getRootConfigPath() + "/tool/TEMPLATE_tool.xml";
    return retval;
}
*/

}
