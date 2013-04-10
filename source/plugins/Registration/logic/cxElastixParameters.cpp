// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxElastixParameters.h"

#include <QDir>
#include "sscLogger.h"
#include "cxDataLocations.h"
#include "cxSettings.h"

namespace cx
{

ElastixParameters::ElastixParameters(ssc::XmlOptionFile options)
{
    mOptions = options;

    mCurrentPreset = ssc::StringDataAdapterXml::initialize("currentPreset", "Preset", "Current Elastix Preset", "Select Preset...", QStringList(), mOptions.getElement());
    connect(mCurrentPreset.get(), SIGNAL(changed()), this, SLOT(currentPresetChangedSlot()));

    this->currentPresetChangedSlot();
}

void ElastixParameters::addDefaultPresets()
{
    QString defaultExecutable = cx::DataLocations::getRootConfigPath() + "/elastix/bin/run_elastix.sh";
    this->addDefaultPreset("elastix/p_Rigid", defaultExecutable, QStringList() << "p_Rigid.txt");
}

void ElastixParameters::addDefaultPreset(QString name, QString executable, QStringList parameterFiles)
{
    // ignore add if already present:
    if (!mOptions.tryDescend("preset", "name", name).isNull())
        return;

    ssc::XmlOptionFile node = mOptions.descend("preset", "name", name);
    node.getElement().setAttribute("executable", executable);
    for (unsigned i=0; i<parameterFiles.size(); ++i)
    {
        QString parName = QString("parameterFile%1").arg(i);
        QString parVal = QFileInfo(parameterFiles[i]).fileName();
        node.getElement().setAttribute(parName, parVal);
    }
}

void ElastixParameters::currentPresetChangedSlot()
{
    this->reloadPresets();

    ssc::XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
    mActiveExecutable = node.getElement().attribute("executable");
    mActiveParameterFile0 = this->getFullParameterFilename(node.getElement().attribute("parameterFile0"));
    mActiveParameterFile1 = this->getFullParameterFilename(node.getElement().attribute("parameterFile1"));
    emit elastixParametersChanged();
}

QString ElastixParameters::getFullParameterFilename(QString filename)
{
    if (filename.isEmpty())
        return "";
    QDir dir(this->getParameterFilesDir());
    return dir.filePath(filename);
}

QString ElastixParameters::getParameterFilesDir() const
{
    return cx::DataLocations::getRootConfigPath() + "/elastix/parameterFiles";
}

ssc::StringDataAdapterPtr ElastixParameters::getCurrentPreset()
{
    return mCurrentPreset;
}

void ElastixParameters::removeCurrentPreset()
{
    ssc::XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
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
    ssc::XmlOptionFile node = mOptions.descend("preset", "name", name);
    node.getElement().setAttribute("executable", mActiveExecutable);
    node.getElement().setAttribute("parameterFile0", QFileInfo(mActiveParameterFile0).fileName());
    node.getElement().setAttribute("parameterFile1", QFileInfo(mActiveParameterFile1).fileName());
    mCurrentPreset->setValue(name);
}

void ElastixParameters::setActiveParameterFile0(QString filename)
{
    mActiveParameterFile0 = filename;
    emit elastixParametersChanged();
}

QString ElastixParameters::getActiveParameterFile0() const
{
    return mActiveParameterFile0;
}

void ElastixParameters::setActiveParameterFile1(QString filename)
{
    mActiveParameterFile1 = filename;
    emit elastixParametersChanged();
}

QString ElastixParameters::getActiveParameterFile1() const
{
    return mActiveParameterFile1;
}

void ElastixParameters::setActiveExecutable(QString filename)
{
    mActiveExecutable = filename;
    emit elastixParametersChanged();
}

QString ElastixParameters::getActiveExecutable() const
{
    return mActiveExecutable;
}

QStringList ElastixParameters::getActiveParameterFiles() const
{
    QStringList retval;
    if (QFileInfo(mActiveParameterFile0).exists() && QFileInfo(mActiveParameterFile0).isFile())
        retval << mActiveParameterFile0;
    if (QFileInfo(mActiveParameterFile1).exists() && QFileInfo(mActiveParameterFile1).isFile())
        retval  << mActiveParameterFile1;
    return retval;
}

QString ElastixParameters::getPresetNameSuggesion() const
{
    QString retval = QFileInfo(mActiveExecutable).baseName();
    QStringList parFiles = this->getActiveParameterFiles();
    for (unsigned i=0; i<parFiles.size(); ++i)
        retval += "/" + QFileInfo(parFiles[i]).baseName();
    return retval;
}

} /* namespace cx */
