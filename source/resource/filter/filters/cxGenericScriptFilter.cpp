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

StringPropertyPtr GenericScriptFilter::getParameterFile(QDomElement root)
{
	QStringList availableScripts;

	// Search for .ini files in folder
	// Scan .ini files for references to scripts

	if(availableScripts.isEmpty())
	{
		availableScripts << "No script available";
	}
	return StringProperty::initialize("scriptSelector",
													"Script",
													"Select which script to use.",
													availableScripts[0],
													availableScripts,
													root);
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

void GenericScriptFilter::createOptions()
{
	// Need file selector, editable text field and output (optional?).

	mOptionsAdapters.push_back(this->getParameterFile(mOptions));
	mOptionsAdapters.push_back(this->getScriptContent(mOptions));
	mOptionsAdapters.push_back(this->setScriptOutput(mOptions));

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

