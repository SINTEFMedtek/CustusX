/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGenericScriptFilter.h"

#include "cxAlgorithmHelpers.h"
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include "cxSelectDataStringProperty.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxImage.h"

#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"
#include "cxFilePreviewProperty.h"
#include "cxFilePathProperty.h"
#include "cxProfile.h"
#include "cxLogger.h"

namespace cx
{

GenericScriptFilter::GenericScriptFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString GenericScriptFilter::getName() const
{
	return "Script";
}

QString GenericScriptFilter::getType() const
{
	return "generic_script_filter";
}

QString GenericScriptFilter::getHelp() const
{
	return "<html>"
			"<h3>Script.</h3>"
			"<p>Support for calling external scripts from Custus"
			"<p>Uses parameter file... "
			"....</p>"
	        "</html>";
}

FilePathPropertyPtr GenericScriptFilter::getParameterFile(QDomElement root)
{
	QStringList availableScripts;

	// Search for .ini files in folder
	// Scan .ini files for references to scripts

	if(availableScripts.isEmpty())
	{
		availableScripts << "No script available";
	}
	mScriptFile =  FilePathProperty::initialize("scriptSelector",
													"Script",
													"Select which script to use.",
													availableScripts[0],//FilePath
													availableScripts,
													root);
	mScriptFile->setGroup("File");
	connect(mScriptFile.get(), &FilePathProperty::changed, this, &GenericScriptFilter::scriptFileChanged);
	return mScriptFile;
}

StringPropertyPtr GenericScriptFilter::getScriptContent(QDomElement root)
{
	QString content;
	content.append("File contents goes here.");
	content.append("More text.");
	return StringProperty::initialize("terminalOutput", "Content",
												 "Used for...",
												 content, root);

	// This should be repaced with cxFilePreviewWidget ?
}

StringPropertyPtr GenericScriptFilter::setScriptOutput(QDomElement root)
{
	QString scriptOutput;
	scriptOutput.append("File output goes here.");
	scriptOutput.append("More text.");

	return StringProperty::initialize("terminalOutput", "Script output",
												 "Used for...",
												 scriptOutput, root);

	// How to set multiline output?
}

FilePreviewPropertyPtr GenericScriptFilter::getIniFileOption(QDomElement root)
{
	QStringList paths;
	paths << profile()->getSettingsPath();

	mScriptFilePreview = FilePreviewProperty::initialize("filename", "Filename",
											"Select a ini file for running command line script",
											mScriptFile->getValue(),
											paths,
											root);

	mScriptFilePreview->setGroup("File");
	this->scriptFileChanged();//Initialize with data from mScriptFile variable
	return mScriptFilePreview;
}

void GenericScriptFilter::createOptions()
{
	// Need file selector, editable text field and output (optional?).

	mOptionsAdapters.push_back(this->getParameterFile(mOptions));
	mOptionsAdapters.push_back(this->getScriptContent(mOptions));
	mOptionsAdapters.push_back(this->getIniFileOption(mOptions));
	// mOptionsAdapters.push_back(this->setScriptOutput(mOptions));

}

void GenericScriptFilter::scriptFileChanged()
{
	CX_LOG_DEBUG() << "scriptFileChanged: " << mScriptFile->getValue();
	//Don't work. Need to get property from options, or trigger another signal?
	mScriptFilePreview->setValue(mScriptFile->getValue());
}

void GenericScriptFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select image input");
	mInputTypes.push_back(temp);
}

void GenericScriptFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output smoothed image");
	mOutputTypes.push_back(temp);

}

bool GenericScriptFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	// Start script, wait for finish?
	// Get command line output
	return true;
}

bool GenericScriptFilter::postProcess()
{
	// Check resulting file (if created)

	if (!mRawResult)
		return false;

//	ImagePtr input = this->getCopiedInputImage();

//	if (!input)
//		return false;

//	QString uid = input->getUid() + "_sm%1";
//	QString name = input->getName()+" sm%1";
//	ImagePtr output = createDerivedImage(mServices->patient(),
//										 uid, name,
//										 mRawResult, input);

//	mRawResult = NULL;
//	if (!output)
//		return false;

//	mServices->patient()->insertData(output);

//	// set output
//	mOutputTypes.front()->setValue(output->getUid());

	return true;

}


} // namespace cx

