/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxElastixParameters.h"

#include <QDir>

#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxTypeConversions.h"
#include "cxProfile.h"
#include "cxFilePathProperty.h"
#include "cxLogger.h"
#include <QApplication>

namespace cx
{

ElastixParameters::ElastixParameters(XmlOptionFile options)
{
    mOptions = options;

	mActiveExecutable = this->getExecutable();
	mActiveParameterFile0 = this->getParameterFile("0");
	mActiveParameterFile1 = this->getParameterFile("1");

	mCurrentPreset = StringProperty::initialize("currentPreset", "Preset",
												"Current Preset", "Select Preset...",
												QStringList(), mOptions.getElement());
    connect(mCurrentPreset.get(), SIGNAL(changed()), this, SLOT(currentPresetChangedSlot()));

    this->currentPresetChangedSlot();
}

void ElastixParameters::addDefaultPresets()
{
    FilePathPropertyPtr exe = this->getExecutable();

    if (DataLocations::isRunFromBuildFolder())
    {
        exe->setValue(cx::DataLocations::findConfigFilePath("run_elastix.sh", this->getConfigUid()+"/elastix/bin"));
    }
    else
    {
        exe->setValue(DataLocations::findExecutableInStandardLocations("elastix"));
    }

    FilePathPropertyPtr par0 = this->getParameterFile("0");
    par0->setValue("elastix/par/p_Rigid.txt");

    this->addDefaultPreset("elastix/p_Rigid", exe->getValue(), QStringList() << par0->getValue());
}

void ElastixParameters::addDefaultPreset(QString name, QString executable, QStringList parameterFiles)
{	
    // ignore add if already present:
    if (!mOptions.tryDescend("preset", "name", name).isNull())
        return;

    XmlOptionFile node = mOptions.descend("preset", "name", name);
    node.getElement().setAttribute("executable", executable);
	for (unsigned i=0; i<parameterFiles.size(); ++i)
	{
		QString parName = QString("parameterFile%1").arg(i);
		QString parVal = parameterFiles[i];
		node.getElement().setAttribute(parName, parVal);
	}
}

void ElastixParameters::currentPresetChangedSlot()
{
    this->reloadPresets();

    XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
	mActiveExecutable->setValue(node.getElement().attribute("executable"));

	mActiveParameterFile0->setValue(node.getElement().attribute("parameterFile0"));
	mActiveParameterFile1->setValue(node.getElement().attribute("parameterFile1"));

	emit elastixParametersChanged();
}

QString ElastixParameters::getFullParameterFilename(QString filename)
{
	return DataLocations::findConfigFilePath(filename, this->getConfigUid());
}

QString ElastixParameters::getConfigUid()
{
	return "org.custusx.registration.method.commandline";
}

StringPropertyBasePtr ElastixParameters::getCurrentPreset()
{
    return mCurrentPreset;
}

void ElastixParameters::removeCurrentPreset()
{
    XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
    node.deleteNode();
    this->reloadPresets();
    mCurrentPreset->setValue("Select Preset...");
}

void ElastixParameters::reloadPresets()
{
    this->addDefaultPresets();
    QStringList presets;
    presets << "Select Preset...";

    QDomNodeList presetNodeList = mOptions.getElement().elementsByTagName("preset");
    for (int i = 0; i < presetNodeList.count(); ++i)
    {
        presets << presetNodeList.item(i).toElement().attribute("name");
    }
    presets.removeDuplicates();

    mCurrentPreset->blockSignals(true);
    mCurrentPreset->setValueRange(presets);
    mCurrentPreset->blockSignals(false);
}

void ElastixParameters::saveCurrentPreset(QString name)
{
    XmlOptionFile node = mOptions.descend("preset", "name", name);
	node.getElement().setAttribute("executable", mActiveExecutable->getEmbeddedPath().getRelativeFilepath());
	node.getElement().setAttribute("parameterFile0", mActiveParameterFile0->getEmbeddedPath().getRelativeFilepath());
	node.getElement().setAttribute("parameterFile1", mActiveParameterFile1->getEmbeddedPath().getRelativeFilepath());
    mCurrentPreset->setValue(name);
}

FilePathPropertyPtr ElastixParameters::getExecutable()
{
    QStringList paths;
    if (DataLocations::isRunFromBuildFolder())
    {
        paths = DataLocations::getRootConfigPaths();
        paths = DataLocations::appendStringToAllElements(paths, "/"+this->getConfigUid());
    }
    else
    {
        paths << qApp->applicationDirPath();
    }

	QDomElement root;
	FilePathPropertyPtr retval;
	retval = FilePathProperty::initialize("executable", "Executable",
										  "Name of registration executable",
										  "",
										  paths,
										  root);
	connect(retval.get(), &FilePathProperty::changed, this, &ElastixParameters::elastixParametersChanged);
	return retval;
}

FilePathPropertyPtr ElastixParameters::getParameterFile(QString uid)
{
	QStringList paths = DataLocations::getRootConfigPaths();
	paths = DataLocations::appendStringToAllElements(paths, "/"+this->getConfigUid());

	QDomElement root;
	FilePathPropertyPtr retval;
	retval = FilePathProperty::initialize("parameter"+uid, "Parameter"+uid,
										  "Name of parameter file "+uid,
										  "",
										  paths,
										  root);
	connect(retval.get(), &FilePathProperty::changed, this, &ElastixParameters::elastixParametersChanged);
	return retval;
}

bool ElastixParameters::validParameterFile(QString file) const
{
    return QFileInfo(file).exists() && QFileInfo(file).isFile();
}

QStringList ElastixParameters::getActiveParameterFiles() const
{
	QString p0 = mActiveParameterFile0->getEmbeddedPath().getAbsoluteFilepath();
	QString p1 = mActiveParameterFile1->getEmbeddedPath().getAbsoluteFilepath();

    QStringList retval;
	if (this->validParameterFile(p0))
		retval << p0;
	if (this->validParameterFile(p1))
		retval  << p1;
    return retval;
}

QString ElastixParameters::getPresetNameSuggesion() const
{
	QString retval = QFileInfo(mActiveExecutable->getValue()).baseName();
    QStringList parFiles = this->getActiveParameterFiles();
    for (unsigned i=0; i<parFiles.size(); ++i)
        retval += "/" + QFileInfo(parFiles[i]).baseName();
    return retval;
}

} /* namespace cx */
