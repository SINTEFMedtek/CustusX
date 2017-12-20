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
		ToolStructure() :
			mAbsoluteToolFilePath(""),
			mOpenIGTLinkTransformId(""),
			mOpenIGTLinkImageId(""),
			mReference(false)
		{}
	};


	typedef std::vector<ToolStructure> ToolStructureVector;
	typedef std::map<TRACKING_SYSTEM, ToolStructureVector> TrackersAndToolsMap;
	struct Configuration
	{
		QString mFileName; ///< absolute path and filename for the new config file
		QString mClinical_app; ///< the clinical application this config is made for
		QString mTrackingSystemName;
		TrackersAndToolsMap mTrackersAndTools; ///< the trackers and tools (relative path) that should be used in the config
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

	QString getTrackingSystem();
private:
	void setConfigDocument(QString configAbsoluteFilePath);
	bool isConfigFileValid();
	QString getAbsoluteToolFilePath(QDomElement toolfileelement); ///< get the absolute toolfile path for a toolfile element containg a relative toolfile path
	QString findXmlFileWithDirNameInPath(QString path);
	QString searchForExistingToolFilePath(QString relativeToolFilePath);
	static QString convertToRelativeToolFilePath(QString configFilename, QString absoluteToolFilePath);
	static QString getToolPathFromRoot(QString root);

	QString mConfigurationFilePath; ///< absolute path to the configuration file
	QString mLoggingFolder; ///< absolutepath to the logging folder

	QDomDocument mConfigureDoc; ///< the config xml document
};


/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */
