#include "cxIGSTKToolFileParser.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include "cxLogger.h"
#include "cxEnumConverter.h"
#include "cxFrame3D.h"
#include "cxTransformFile.h"

namespace cx {

IGSTKToolFileParser::IGSTKToolFileParser(QString absoluteToolFilePath, QString loggingFolder) :
    ToolFileParser(absoluteToolFilePath, loggingFolder)
{
}

ToolFileParser::ToolInternalStructure IGSTKToolFileParser::getTool()
{
    ToolInternalStructure retval;

    QFile toolFile(mToolFilePath);
    QString toolFolderAbsolutePath = QFileInfo(toolFile).dir().absolutePath() + "/";
    QDomNode toolNode = this->getToolNode(mToolFilePath);
    ToolFileParser::ToolInternalStructure internalStructure;
    if (toolNode.isNull())
    {
        report(
                        "Could not read the <tool> tag of file: " + mToolFilePath
                                        + ", this is not a tool file, skipping.");
        return retval;
    }

    QDomElement toolTypeElement = toolNode.firstChildElement(mToolTypeTag);
    QString toolTypeText = toolTypeElement.text();

    internalStructure.mIsReference = toolTypeText.contains("reference", Qt::CaseInsensitive);
    internalStructure.mIsPointer = toolTypeText.contains("pointer", Qt::CaseInsensitive);
    internalStructure.mIsProbe = toolTypeText.contains("usprobe", Qt::CaseInsensitive);

//    if (toolTypeText.contains("reference", Qt::CaseInsensitive))
//    {
//      internalStructure.mType = Tool::TOOL_REFERENCE;
//    } else if (toolTypeText.contains("pointer", Qt::CaseInsensitive))
//    {
//      internalStructure.mType = Tool::TOOL_POINTER;
//    } else if (toolTypeText.contains("usprobe", Qt::CaseInsensitive))
//    {
//      internalStructure.mType = Tool::TOOL_US_PROBE;
//    } else
//    {
//      internalStructure.mType = Tool::TOOL_NONE;
//    }

    QDomElement toolIdElement = toolNode.firstChildElement(mToolIdTag);
    QString toolIdText = toolIdElement.text();
    internalStructure.mUid = toolIdText;

    QDomElement toolNameElement = toolNode.firstChildElement(mToolNameTag);
    QString toolNameText = toolNameElement.text();
    internalStructure.mName = toolNameText;

    QDomElement toolClinicalAppElement = toolNode.firstChildElement(mToolClinicalAppTag);
    QString toolClinicalAppText = toolClinicalAppElement.text();
    QStringList applicationList = toolClinicalAppText.split(" ");
    foreach(QString string, applicationList)
    {
        if (string.isEmpty())
            continue;
        string = string.toLower();
        internalStructure.mClinicalApplications.push_back(string);
    }

    QDomElement toolGeofileElement = toolNode.firstChildElement(mToolGeoFileTag);
    QString toolGeofileText = toolGeofileElement.text();
    if (!toolGeofileText.isEmpty())
        toolGeofileText = toolFolderAbsolutePath + toolGeofileText;
    internalStructure.mGraphicsFileName = toolGeofileText;

    QDomElement toolPicfileElement = toolNode.firstChildElement(mToolPicFileTag);
    QString toolPicfileText = toolPicfileElement.text();
    if (!toolPicfileText.isEmpty())
        toolPicfileText = toolFolderAbsolutePath + toolPicfileText;
    internalStructure.mPictureFileName = toolPicfileText;

    QDomElement toolInstrumentElement = toolNode.firstChildElement(mToolInstrumentTag);
    if (toolInstrumentElement.isNull())
    {
        reportError(
                        "Could not find the <instrument> tag under the <tool> tag. Aborting this tool.");
        return retval;
    }
    QDomElement toolInstrumentIdElement = toolInstrumentElement.firstChildElement(mToolInstrumentIdTag);
    QString toolInstrumentIdText = toolInstrumentIdElement.text();
    internalStructure.mInstrumentId = toolInstrumentIdText;

    QDomElement toolInstrumentScannerIdElement = toolInstrumentElement.firstChildElement(mToolInstrumentScannerIdTag);
    QString toolInstrumentScannerIdText = toolInstrumentScannerIdElement.text();
    internalStructure.mInstrumentScannerId = toolInstrumentScannerIdText;

    QDomElement toolSensorElement = toolNode.firstChildElement(mToolSensorTag);
    if (toolSensorElement.isNull())
    {
        reportError("Could not find the <sensor> tag under the <tool> tag. Aborting this tool.");
        return retval;
    }
    QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(mToolSensorTypeTag);
    QString toolSensorTypeText = toolSensorTypeElement.text();
    internalStructure.mTrackerType = string2enum<TRACKING_SYSTEM>(toolSensorTypeText);

    QDomElement toolSensorWirelessElement = toolSensorElement.firstChildElement(mToolSensorWirelessTag);
    QString toolSensorWirelessText = toolSensorWirelessElement.text();
    if (toolSensorWirelessText.contains("yes", Qt::CaseInsensitive))
        internalStructure.mWireless = true;
    else if (toolSensorWirelessText.contains("no", Qt::CaseInsensitive))
        internalStructure.mWireless = false;

    QDomElement toolSensorDOFElement = toolSensorElement.firstChildElement(mToolSensorDOFTag);
    QString toolSensorDOFText = toolSensorDOFElement.text();
    if (toolSensorDOFText.contains("5", Qt::CaseInsensitive))
        internalStructure.m5DOF = true;
    else if (toolSensorDOFText.contains("6", Qt::CaseInsensitive))
        internalStructure.m5DOF = false;

    QDomElement toolSensorPortnumberElement = toolSensorElement.firstChildElement(mToolSensorPortnumberTag);
    QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
    internalStructure.mPortNumber = toolSensorPortnumberText.toInt();

    QDomElement toolSensorChannelnumberElement = toolSensorElement.firstChildElement(mToolSensorChannelnumberTag);
    QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
    internalStructure.mChannelNumber = toolSensorChannelnumberText.toInt();

    QDomNodeList toolSensorReferencePointList = toolSensorElement.elementsByTagName(mToolSensorReferencePointTag);
    for (int j = 0; j < toolSensorReferencePointList.count(); j++)
    {
        QDomNode node = toolSensorReferencePointList.item(j);
        if (!node.hasAttributes())
        {
            reportWarning("Found reference point without id attribute. Skipping.");
            continue;
        }
        bool ok;
        int id = node.toElement().attribute("id").toInt(&ok);
        if (!ok)
        {
            reportWarning("Attribute id of a reference point was not an int. Skipping.");
            continue;
        }
        QString toolSensorReferencePointText = node.toElement().text();
        Vector3D vector = Vector3D::fromString(toolSensorReferencePointText);
        internalStructure.mReferencePoints[id] = vector;
    }

    QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(mToolSensorRomFileTag);
    QString toolSensorRomFileText = toolSensorRomFileElement.text();
    if (!toolSensorRomFileText.isEmpty())
        toolSensorRomFileText = toolFolderAbsolutePath + toolSensorRomFileText;
    internalStructure.mSROMFilename = toolSensorRomFileText;

    QDomElement toolCalibrationElement = toolNode.firstChildElement(mToolCalibrationTag);
    if (toolCalibrationElement.isNull())
    {
        reportError(
                        "Could not find the <calibration> tag under the <tool> tag. Aborting this tool.");
        return retval;
    }
    QDomElement toolCalibrationFileElement = toolCalibrationElement.firstChildElement(mToolCalibrationFileTag);
    QString toolCalibrationFileText = toolCalibrationFileElement.text();
    if (!toolCalibrationFileText.isEmpty())
        toolCalibrationFileText = toolFolderAbsolutePath + toolCalibrationFileText;
    internalStructure.mCalibrationFilename = toolCalibrationFileText;
    internalStructure.mCalibration = this->readCalibrationFile(internalStructure.mCalibrationFilename);

    internalStructure.mTransformSaveFileName = mLoggingFolder;
    internalStructure.mLoggingFolderName = mLoggingFolder;
    retval = internalStructure;

    return retval;
}

Transform3D IGSTKToolFileParser::readCalibrationFile(QString absoluteFilePath)
{
    bool ok = true;
    TransformFile file(absoluteFilePath);
    Transform3D retval = file.read(&ok);

    if (ok)
    {
        retval = Frame3D::create(retval).transform(); // clean rotational parts, transform should now be pure rotation+translation
    }

    return retval;
}
} //namespace cx


