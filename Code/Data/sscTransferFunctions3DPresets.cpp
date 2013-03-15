// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscTransferFunctions3DPresets.h"

#include <iostream>
#include <vtkColorTransferFunction.h>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QColor>
#include "sscTypeConversions.h"
#include "sscXmlOptionItem.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include <sscImageTFData.h>

namespace ssc
{

TransferFunctions3DPresets::TransferFunctions3DPresets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile) :
		Presets(presetFile, customFile)
{
//	mPresetFile = presetFile;
//	mCustomFile = customFile;

	//mPresetFile = ssc::XmlOptionFile(DataLocations::getRootConfigPath()+"/transferFunctions/presets.xml", "transferFunctions"); ///< create from filename, create trivial document of type name and root node if no file exists.
}

//QStringList TransferFunctions3DPresets::getPresetList(QString modality)
//{
//	return this->generatePresetList(modality);
//}

//ssc::XmlOptionFile TransferFunctions3DPresets::getCustomFile()
//{
//	return mCustomFile;
//	//	return ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("presetTransferFunctions");
//}

void TransferFunctions3DPresets::save(QString name, ssc::ImagePtr image, bool _2D, bool _3D)
{
	if (_2D)
		this->save2D(name, image);
	if (_3D)
		this->save3D(name, image);
}


void TransferFunctions3DPresets::save2D(QString name, ssc::ImagePtr image)
{
	ssc::XmlOptionFile file = this->getCustomFile();
	file = file.descend("Preset", "name", name);

	//TODO
//	ssc::XmlOptionFile file = Presets::saveCustom(name);

	QDomNode tf2DNode = file.getElement("lookuptable2D");
	while (tf2DNode.hasChildNodes())
		tf2DNode.removeChild(tf2DNode.firstChild());

	ssc::ImageLUT2DPtr LUT2D = image->getLookupTable2D();

	// For unsigned CT: Modify transfer function values temporarily prior to save
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		LUT2D->unsignedCT(false);
	}

	LUT2D->addXml(file.getElement("lookuptable2D"));

	// Revert the transfer function values back again
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		LUT2D->unsignedCT(true);
	}

	file.getElement().setAttribute("modality", image->getModality());
	file.save();

//	emit changed();
}

void TransferFunctions3DPresets::save3D(QString name, ssc::ImagePtr image)
{
	//create the node to be saved
	ssc::XmlOptionFile file = this->getCustomFile();
	file = file.descend("Preset", "name", name);

	QDomNode tf3DNode = file.getElement("transferfunctions");
	while (tf3DNode.hasChildNodes())
		tf3DNode.removeChild(tf3DNode.firstChild());

	ssc::ImageTF3DPtr transferFunctions = image->getTransferFunctions3D();

	// For unsigned CT: Modify transfer function values temporarily prior to save
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		transferFunctions->unsignedCT(false);
	}

	//generate xml
//	QDomElement transferfunctionElement = QDomDocument().createElement("transferfunctions");
//	QDomElement shadingElement = QDomDocument().createElement("shading");
//	transferFunctions->addXml(transferfunctionElement);
//	image->getShading().addXml(shadingElement);

	transferFunctions->addXml(file.getElement("transferfunctions"));
	image->getShading().addXml(file.getElement("shading"));

	// Revert the transfer function values back again
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		transferFunctions->unsignedCT(true);
	}

	file.getElement().setAttribute("modality", image->getModality());
	file.save();

//	//save it
//	std::vector<QDomElement> children;
//	children.push_back(transferfunctionElement);
//	children.push_back(shadingElement);
//	std::map<QString, QString> attributes;
//	attributes["modality"] = image->getModality();
//	ssc::XmlOptionFile file = Presets::saveCustom(name, attributes, children);
}

void TransferFunctions3DPresets::load(QString name, ssc::ImagePtr image, bool _2D, bool _3D)
{
	if (_2D)
		this->load2D(name, image);
	if (_3D)
		this->load3D(name, image);
}

//void PresetTransferFunctions3D::load(QString name, ssc::ImagePtr image, bool _2D, bool _3D)
//{
//	if (_2D)
//
//
//	//Make sure transfer functions are reset in case something is missing from the preset
//	image->resetTransferFunctions();
//
//  if(name == "Transfer function preset...")
//  	return;
//
//	ssc::ImageTF3DPtr transferFunctions = image->getTransferFunctions3D();
//	ssc::ImageLUT2DPtr LUT2D = image->getLookupTable2D();
//	ssc::XmlOptionFile node = this->getPresetNode(name);
//
//	transferFunctions->parseXml(node.getElement().namedItem("transferfunctions"));
//	LUT2D->parseXml(node.getElement().namedItem("lookuptable2D"));
//
//	ssc::Image::ShadingStruct shading = image->getShading();
//	shading.parseXml(node.getElement().namedItem("shading"));
//	image->setShading(shading);
//
//	// Transfer functions for CT data are signed, so these have to be converted if they are to be used for unsigned CT
//	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
//	{
//		transferFunctions->unsignedCT(true);
//		LUT2D->unsignedCT(true);
//	}
//
//	//Make sure the preset transfer functions work correctly
//	transferFunctions->fixTransferFunctions();
//	LUT2D->fixTransferFunctions();
//
////	emit changed();
//}

void TransferFunctions3DPresets::load2D(QString name, ssc::ImagePtr image)
{
	//Make sure transfer functions are reset in case something is missing from the preset
	image->resetTransferFunctions(true, false);

	ssc::ImageLUT2DPtr LUT2D = image->getLookupTable2D();
	ssc::XmlOptionFile node = this->getPresetNode(name);

	LUT2D->parseXml(node.getElement().namedItem("lookuptable2D"));

	// Transfer functions for CT data are signed, so these have to be converted if they are to be used for unsigned CT
	if ((0 <= image->getMin()) && ("CT" == image->getModality()) && (name != "Transfer function preset...") )
	{
		LUT2D->unsignedCT(true);
	}

	//Make sure the preset transfer functions work correctly
	LUT2D->fixTransferFunctions();
}

void TransferFunctions3DPresets::load3D(QString name, ssc::ImagePtr image)
{
	//Make sure transfer functions are reset in case something is missing from the preset
	image->resetTransferFunctions(false, true);

	ssc::ImageTF3DPtr transferFunctions = image->getTransferFunctions3D();
	ssc::XmlOptionFile node = this->getPresetNode(name);

	transferFunctions->parseXml(node.getElement().namedItem("transferfunctions"));

	ssc::Image::ShadingStruct shading = image->getShading();
	shading.parseXml(node.getElement().namedItem("shading"));
	image->setShading(shading);

	// Transfer functions for CT data are signed, so these have to be converted if they are to be used for unsigned CT
	if ((0 <= image->getMin()) && ("CT" == image->getModality()) && (name != "Transfer function preset...") )
	{
		transferFunctions->unsignedCT(true);
	}

	//Make sure the preset transfer functions work correctly
	transferFunctions->fixTransferFunctions();
}

///** look for a preset with the given name. Create one if not found.
// *
// */
//ssc::XmlOptionFile TransferFunctions3DPresets::getPresetNode(const QString& presetName)
//{
//	ssc::XmlOptionFile retval = mPresetFile;
//	retval = retval.tryDescend("Preset", "name", presetName);
//	if (!retval.getDocument().isNull())
//		return retval;
//
//	retval = this->getCustomFile();
//	retval = retval.descend("Preset", "name", presetName);
//	return retval;
//}

QStringList TransferFunctions3DPresets::generatePresetList(QString modality)
{
	QStringList presetList;
//	presetList.append("Transfer function preset...");

	QDomNodeList presetNodeList = mPresetFile.getElement().elementsByTagName("Preset");
	for (int i = 0; i < presetNodeList.count(); ++i)
	{
		QString presetName = presetNodeList.item(i).toElement().attribute("name");
		if (presetName == "Default")
			continue;
		else
		{
			QString sourceModality = presetNodeList.item(i).toElement().attribute("modality");
			if ( (modality == sourceModality) || ("UNKNOWN" == modality) || modality.isEmpty() )
				presetList << presetName;
		}
	}

	ssc::XmlOptionFile customFile = this->getCustomFile();
	presetNodeList = customFile.getElement().elementsByTagName("Preset");
	for (int i = 0; i < presetNodeList.count(); ++i)
	{
		QString presetName = presetNodeList.item(i).toElement().attribute("name");
		QString presetModality = presetNodeList.item(i).toElement().attribute("modality");
		if ( (presetModality == modality) || ("UNKNOWN" == modality) || modality.isEmpty() )
			presetList << presetName;
	}

	return presetList;
}

//bool TransferFunctions3DPresets::isDefaultPreset(QString presetName)
//{
//	ssc::XmlOptionFile testval = mPresetFile.tryDescend("Preset", "name", presetName);
//	if (!testval.getDocument().isNull())
//		return true;
//	return false;
//}

void TransferFunctions3DPresets::deletePresetData(QString name, bool _2D, bool _3D)
{
	ssc::XmlOptionFile node = this->getPresetNode(name);

	if (_2D)
		node.descend("lookuptable2D").deleteNode();
	if (_3D)
		node.descend("transferfunctions").deleteNode();
	if (_2D && _3D)
		node.deleteNode();

	emit changed();
}

}
