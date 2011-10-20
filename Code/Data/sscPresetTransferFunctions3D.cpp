#include "sscPresetTransferFunctions3D.h"

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

PresetTransferFunctions3D::PresetTransferFunctions3D(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile)
{
	mPresetFile = presetFile;
	mCustomFile = customFile;

	//mPresetFile = ssc::XmlOptionFile(DataLocations::getRootConfigPath()+"/transferFunctions/presets.xml", "transferFunctions"); ///< create from filename, create trivial document of type name and root node if no file exists.
}

PresetTransferFunctions3D::~PresetTransferFunctions3D()
{
}

QStringList PresetTransferFunctions3D::getPresetList(QString modality)
{
	return this->generatePresetList(modality);
}

ssc::XmlOptionFile PresetTransferFunctions3D::getCustomFile()
{
	return mCustomFile;
	//	return ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("presetTransferFunctions");
}

void PresetTransferFunctions3D::save(QString name, ssc::ImagePtr image)
{
	ssc::XmlOptionFile file = this->getCustomFile();
	file = file.descend("Preset", "name", name);

	QDomNode tf3DNode = file.getElement("transferfunctions");
	while (tf3DNode.hasChildNodes())
		tf3DNode.removeChild(tf3DNode.firstChild());
	QDomNode tf2DNode = file.getElement("lookuptable2D");
	while (tf2DNode.hasChildNodes())
		tf2DNode.removeChild(tf2DNode.firstChild());

	ssc::ImageTF3DPtr transferFunctions = image->getTransferFunctions3D();
	ssc::ImageLUT2DPtr LUT2D = image->getLookupTable2D();

	// For unsigned CT: Modify transfer function values temporarily prior to save
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		transferFunctions->unsignedCT(false);
		LUT2D->unsignedCT(false);
	}

	transferFunctions->addXml(file.getElement("transferfunctions"));
	LUT2D->addXml(file.getElement("lookuptable2D"));
	image->getShading().addXml(file.getElement("shading"));

	// Revert the transfer function values back again
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		transferFunctions->unsignedCT(true);
		LUT2D->unsignedCT(true);
	}

	file.getElement().setAttribute("modality", image->getModality());
	file.save();

//	emit changed();
}

void PresetTransferFunctions3D::load(QString name, ssc::ImagePtr image)
{
	//Make sure transfer functions are reset in case something is missing from the preset
	image->resetTransferFunctions();

  if(name == "Transfer function preset...")
  	return;

	ssc::ImageTF3DPtr transferFunctions = image->getTransferFunctions3D();
	ssc::ImageLUT2DPtr LUT2D = image->getLookupTable2D();
	ssc::XmlOptionFile node = this->getPresetNode(name);

	transferFunctions->parseXml(node.getElement().namedItem("transferfunctions"));
	LUT2D->parseXml(node.getElement().namedItem("lookuptable2D"));

	ssc::Image::ShadingStruct shading = image->getShading();
	shading.parseXml(node.getElement().namedItem("shading"));
	image->setShading(shading);

	// Transfer functions for CT data are signed, so these have to be converted if they are to be used for unsigned CT
	if ((0 <= image->getMin()) && ("CT" == image->getModality()))
	{
		transferFunctions->unsignedCT(true);
		LUT2D->unsignedCT(true);
	}

	//Make sure the preset transfer functions work correctly
	transferFunctions->fixTransferFunctions();
	LUT2D->fixTransferFunctions();

//	emit changed();
}

/** look for a preset with the given name. Create one if not found.
 *
 */
ssc::XmlOptionFile PresetTransferFunctions3D::getPresetNode(const QString& presetName)
{
	ssc::XmlOptionFile retval = mPresetFile;
	retval = retval.tryDescend("Preset", "name", presetName);
	if (!retval.getDocument().isNull())
		return retval;

	retval = this->getCustomFile();
	retval = retval.descend("Preset", "name", presetName);
	return retval;
}

QStringList PresetTransferFunctions3D::generatePresetList(QString modality)
{
	QStringList presetList;
	presetList.append("Transfer function preset...");

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

bool PresetTransferFunctions3D::isDefaultPreset(QString presetName)
{
	ssc::XmlOptionFile testval = mPresetFile.tryDescend("Preset", "name", presetName);
	if (!testval.getDocument().isNull())
		return true;
	return false;
}

void PresetTransferFunctions3D::deletePresetData(QString name)
{
	ssc::XmlOptionFile node = this->getPresetNode(name);
	node.deleteNode();
	emit changed();
}

}
