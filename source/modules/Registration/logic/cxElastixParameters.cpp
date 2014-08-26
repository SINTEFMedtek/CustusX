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

#include "cxElastixParameters.h"

#include <QDir>
#include "cxLogger.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxTypeConversions.h"

namespace cx
{

ElastixParameters::ElastixParameters(XmlOptionFile options)
{
    mOptions = options;

    mCurrentPreset = StringDataAdapterXml::initialize("currentPreset", "Preset", "Current Elastix Preset", "Select Preset...", QStringList(), mOptions.getElement());
    connect(mCurrentPreset.get(), SIGNAL(changed()), this, SLOT(currentPresetChangedSlot()));

    this->currentPresetChangedSlot();
}

void ElastixParameters::addDefaultPresets()
{
//test code - inject into jenkins
	QString pathRelativeToConfigRoot = "/elastix/bin";
	QString filename = "run_elastix.sh";
	foreach (QString root, DataLocations::getRootConfigPaths())
	{
		QString path = root + "/" + pathRelativeToConfigRoot + "/" + filename;
		std::cout << "+++ ELASTIX: attempting: " << path << std::endl;
		std::cout << "+++ ELASTIX:  exists:" << QFileInfo(path).exists() << std::endl;
	}

	QString defaultExecutable = cx::DataLocations::getExistingConfigPath("/elastix/bin", "", "run_elastix.sh");
	std::cout << "+++ ELASTIX:  defaultExecutable:" << defaultExecutable << std::endl;

    this->addDefaultPreset("elastix/p_Rigid", defaultExecutable, QStringList() << "p_Rigid.txt");
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
        QString parVal = QFileInfo(parameterFiles[i]).fileName();
        node.getElement().setAttribute(parName, parVal);
    }
}

void ElastixParameters::currentPresetChangedSlot()
{
    this->reloadPresets();

    XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
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

StringDataAdapterPtr ElastixParameters::getCurrentPreset()
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
