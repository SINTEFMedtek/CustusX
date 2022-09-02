#include "cxToolFileParser.h"

#include <QFile>
#include <QDir>
#include "cxLogger.h"
#include "cxFrame3D.h"
#include "cxTransformFile.h"
#include "cxEnumConversion.h"

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
	mToolFilePath(absoluteToolFilePath), mLoggingFolder(loggingFolder), mToolTag("tool"), mToolTypeTag("type"),
	mToolIdTag("id"), mToolNameTag("name"), mToolDescriptionTag("description"), mToolManufacturerTag("manufacturer"),
	mToolClinicalAppTag("clinical_app"), mToolGeoFileTag("geo_file"), mToolPicFileTag("pic_file"),
	mToolDocFileTag("doc_file"), mToolInstrumentTag("instrument"), mToolInstrumentTypeTag("type"),
	mToolInstrumentIdTag("id"), mToolInstrumentNameTag("name"), mToolInstrumentManufacturerTag("manufacturer"),
	mToolInstrumentScannerIdTag("scannerid"), mToolInstrumentDescriptionTag("description"),
	mToolSensorTag("sensor"), mToolSensorTypeTag("type"), mToolSensorIdTag("id"),
	mToolSensorNameTag("name"), mToolSensorWirelessTag("wireless"), mToolSensorDOFTag("DOF"),
	mToolSensorPortnumberTag("portnumber"), mToolSensorChannelnumberTag("channelnumber"),
	mToolSensorReferencePointTag("reference_point"), mToolSensorManufacturerTag("manufacturer"),
	mToolSensorDescriptionTag("description"), mToolSensorRomFileTag("rom_file"), mToolCalibrationTag("calibration"),
	mToolCalibrationFileTag("cal_file"),
	mToolOpenigtlinkImageIdTag("openigtlinkimageid"), mToolOpenigtlinkTransformIdTag("openigtlinktransformid")
{
}

ToolFileParser::~ToolFileParser()
{
}

ToolFileParser::ToolInternalStructurePtr ToolFileParser::getTool()
{
	ToolInternalStructurePtr retval;

	QFile toolFile(mToolFilePath);
	QString toolFolderAbsolutePath = QFileInfo(toolFile).dir().absolutePath() + "/";
	QDomNode toolNode = this->getToolNode(mToolFilePath);
	ToolInternalStructurePtr internalStructure = ToolInternalStructurePtr(new ToolInternalStructure());
	//    ToolFileParser::ToolInternalStructure internalStructure;
	if (toolNode.isNull())
	{
		report(
					"Could not read the <tool> tag of file: " + mToolFilePath
					+ ", this is not a tool file, skipping.");
		return retval;
	}

	QDomElement toolTypeElement = toolNode.firstChildElement(mToolTypeTag);
	QString toolTypeText = toolTypeElement.text();

	internalStructure->mIsReference = toolTypeText.contains("reference", Qt::CaseInsensitive);
	internalStructure->mIsPointer = toolTypeText.contains("pointer", Qt::CaseInsensitive);
	internalStructure->mIsProbe = toolTypeText.contains("usprobe", Qt::CaseInsensitive);

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
	internalStructure->mUid = toolIdText;

	QDomElement toolNameElement = toolNode.firstChildElement(mToolNameTag);
	QString toolNameText = toolNameElement.text();
	internalStructure->mName = toolNameText;

	QDomElement toolClinicalAppElement = toolNode.firstChildElement(mToolClinicalAppTag);
	QString toolClinicalAppText = toolClinicalAppElement.text();
	QStringList applicationList = toolClinicalAppText.split(" ");
	foreach(QString string, applicationList)
	{
		if (string.isEmpty())
			continue;
		string = string.toLower();
		internalStructure->mClinicalApplications.push_back(string);
	}

	QDomElement toolGeofileElement = toolNode.firstChildElement(mToolGeoFileTag);
	QString toolGeofileText = toolGeofileElement.text();
	if (!toolGeofileText.isEmpty())
		toolGeofileText = toolFolderAbsolutePath + toolGeofileText;
	internalStructure->mGraphicsFileName = toolGeofileText;

	QDomElement toolPicfileElement = toolNode.firstChildElement(mToolPicFileTag);
	QString toolPicfileText = toolPicfileElement.text();
	if (!toolPicfileText.isEmpty())
		toolPicfileText = toolFolderAbsolutePath + toolPicfileText;
	internalStructure->mPictureFileName = toolPicfileText;

	QDomElement toolImageIdfileElement = toolNode.firstChildElement(mToolOpenigtlinkImageIdTag);
	QString toolImageIdfileText = toolImageIdfileElement.text();
	internalStructure->mOpenigtlinkImageId = toolImageIdfileText;

	QDomElement toolTransformIdfileElement = toolNode.firstChildElement(mToolOpenigtlinkTransformIdTag);
	QString toolTransformIdfileText = toolTransformIdfileElement.text();
	internalStructure->mOpenigtlinkTransformId = toolTransformIdfileText;

	QDomElement toolInstrumentElement = toolNode.firstChildElement(mToolInstrumentTag);
	if (toolInstrumentElement.isNull())
	{
		reportError(
					"Could not find the <instrument> tag under the <tool> tag. Aborting this tool.");
		return retval;
	}
	QDomElement toolInstrumentIdElement = toolInstrumentElement.firstChildElement(mToolInstrumentIdTag);
	QString toolInstrumentIdText = toolInstrumentIdElement.text();
	internalStructure->mInstrumentId = toolInstrumentIdText;

	QDomElement toolInstrumentScannerIdElement = toolInstrumentElement.firstChildElement(mToolInstrumentScannerIdTag);
	QString toolInstrumentScannerIdText = toolInstrumentScannerIdElement.text();
	internalStructure->mInstrumentScannerId = toolInstrumentScannerIdText;

	QDomElement toolSensorElement = toolNode.firstChildElement(mToolSensorTag);
	if (toolSensorElement.isNull())
	{
		reportError("Could not find the <sensor> tag under the <tool> tag. Aborting this tool.");
		return retval;
	}
	QDomElement toolSensorTypeElement = toolSensorElement.firstChildElement(mToolSensorTypeTag);
	QString toolSensorTypeText = toolSensorTypeElement.text();
	internalStructure->mTrackerType = string2enum<TRACKING_SYSTEM>(toolSensorTypeText);

	QDomElement toolSensorWirelessElement = toolSensorElement.firstChildElement(mToolSensorWirelessTag);
	QString toolSensorWirelessText = toolSensorWirelessElement.text();
	if (toolSensorWirelessText.contains("yes", Qt::CaseInsensitive))
		internalStructure->mWireless = true;
	else if (toolSensorWirelessText.contains("no", Qt::CaseInsensitive))
		internalStructure->mWireless = false;

	QDomElement toolSensorDOFElement = toolSensorElement.firstChildElement(mToolSensorDOFTag);
	QString toolSensorDOFText = toolSensorDOFElement.text();
	if (toolSensorDOFText.contains("5", Qt::CaseInsensitive))
		internalStructure->m5DOF = true;
	else if (toolSensorDOFText.contains("6", Qt::CaseInsensitive))
		internalStructure->m5DOF = false;

	QDomElement toolSensorPortnumberElement = toolSensorElement.firstChildElement(mToolSensorPortnumberTag);
	QString toolSensorPortnumberText = toolSensorPortnumberElement.text();
	internalStructure->mPortNumber = toolSensorPortnumberText.toUInt();

	QDomElement toolSensorChannelnumberElement = toolSensorElement.firstChildElement(mToolSensorChannelnumberTag);
	QString toolSensorChannelnumberText = toolSensorChannelnumberElement.text();
	internalStructure->mChannelNumber = toolSensorChannelnumberText.toUInt();

	QDomNodeList toolSensorReferencePointList = toolSensorElement.elementsByTagName(mToolSensorReferencePointTag);
	for (int j = 0; j < toolSensorReferencePointList.count(); j++)
	{
		QDomNode node = toolSensorReferencePointList.item(j);
		if (!node.hasAttributes())
		{
			reportWarning("Found reference point without id attribute. Skipping.");
			continue;
		}
		QString id = node.toElement().attribute("id");
		QString toolSensorReferencePointText = node.toElement().text();
		Vector3D vector = Vector3D::fromString(toolSensorReferencePointText);
		internalStructure->mReferencePoints[id] = vector;
	}

	QDomElement toolSensorRomFileElement = toolSensorElement.firstChildElement(mToolSensorRomFileTag);
	QString toolSensorRomFileText = toolSensorRomFileElement.text();
	if (!toolSensorRomFileText.isEmpty())
		toolSensorRomFileText = toolFolderAbsolutePath + toolSensorRomFileText;
	internalStructure->mSROMFilename = toolSensorRomFileText;

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
	internalStructure->mCalibrationFilename = toolCalibrationFileText;
	internalStructure->mCalibration = this->readCalibrationFile(internalStructure->mCalibrationFilename);

	internalStructure->mTransformSaveFileName = mLoggingFolder;
	internalStructure->mLoggingFolderName = mLoggingFolder;
	retval = internalStructure;

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

Transform3D ToolFileParser::readCalibrationFile(QString absoluteFilePath)
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

/*
QString ToolFileParser::getTemplatesAbsoluteFilePath()
{
	QString retval = DataLocations::getRootConfigPath() + "/tool/TEMPLATE_tool.xml";
	return retval;
}
*/

}
