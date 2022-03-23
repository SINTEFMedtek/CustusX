/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLCONFIGURATIONPARSER_H_
#define CXTOOLCONFIGURATIONPARSER_H_

#include "cxResourceExport.h"

#include <utility>
#include <map>
#include <vector>
#include <QString>
#include <QDomDocument>
#include "cxDefinitions.h"
#include "cxToolFileParser.h"

namespace cx
{

/**
 * \brief Class for reading the files defining a CustusX tool
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT ConfigurationFileParser
{
public:

	struct cxResource_EXPORT ToolStructure
	{
		QString mAbsoluteToolFilePath;
		QString mOpenIGTLinkTransformId;
		QString mOpenIGTLinkImageId;
		bool mReference;
		bool mApplyRefToTool;
		ToolStructure() :
			mAbsoluteToolFilePath(""),
			mOpenIGTLinkTransformId(""),
			mOpenIGTLinkImageId(""),
			mReference(false),
			mApplyRefToTool(false)
		{}
	};


	typedef std::vector<ToolStructure> ToolStructureVector;
	typedef std::map<TRACKING_SYSTEM, ToolStructureVector> TrackersAndToolsMap;
	struct Configuration
	{
		QString mFileName; ///< absolute path and filename for the new config file
		QString mClinical_app; ///< the clinical application this config is made for
		QString mTrackingSystemImplementation;//Tracking system implementation (igstk or openigtlink)
		TrackersAndToolsMap mTrackersAndTools; ///< the trackers and tools (relative path) that should be used in the config
		bool mApplyRefToTools;
	};

public:
	ConfigurationFileParser(QString absoluteConfigFilePath, QString loggingFolder = "");
	~ConfigurationFileParser();

	QString getApplicationapplication();
    std::vector<ToolFileParser::TrackerInternalStructure> getTrackers();
	std::vector<QString> getAbsoluteToolFilePaths();
	QString getAbsoluteReferenceFilePath();
	std::vector<ConfigurationFileParser::ToolStructure> getToolListWithMetaInformation();

	static QString getTemplatesAbsoluteFilePath();
	static void saveConfiguration(Configuration& config);

	QString getTrackingSystemImplementation();
	bool getApplyRefToTools();
private:
	void setConfigDocument(QString configAbsoluteFilePath);
	bool isConfigFileValid();
	QString getAbsoluteToolFilePath(QDomElement toolfileelement); ///< get the absolute toolfile path for a toolfile element containg a relative toolfile path
	QString findXmlFileWithDirNameInPath(QString path);
	QString searchForExistingToolFilePath(QString relativeToolFilePath);
	static QString convertToRelativeToolFilePath(QString configFilename, QString absoluteToolFilePath);
	static QString getToolPathFromRoot(QString root);
	static void createToolFileNode(ToolStructureVector::iterator iter, QDomElement &toolFileNode, ToolFileParser &toolparser);

	QString mConfigurationFilePath; ///< absolute path to the configuration file
	QString mLoggingFolder; ///< absolutepath to the logging folder

	QDomDocument mConfigureDoc; ///< the config xml document
};


/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
