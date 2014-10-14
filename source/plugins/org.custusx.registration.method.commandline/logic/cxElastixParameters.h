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
#ifndef CXELASTIXPARAMETERS_H
#define CXELASTIXPARAMETERS_H

#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxStringDataAdapterXml.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

/**
 * \brief Parameter and preset handling for the ElastiX registration.
 *
 * \date Apr 10, 2013
 * \author Christian Askeland, SINTEF
 */
class ElastixParameters : public QObject
{
    Q_OBJECT
public:
    ElastixParameters(XmlOptionFile options);
    StringDataAdapterPtr getCurrentPreset();
    void removeCurrentPreset(); ///< Remove the currently selected preset. Reload.
    void saveCurrentPreset(QString newName);

    void setActiveParameterFile0(QString filename);
    QString getActiveParameterFile0() const;
    void setActiveParameterFile1(QString filename);
    QString getActiveParameterFile1() const;
    void setActiveExecutable(QString filename);
    QString getActiveExecutable() const;

    QStringList getActiveParameterFiles() const;
    QString getPresetNameSuggesion() const; ///< create a name describing the active state, can be used as name for a new preset.
	QStringList getParameterFilesDir() const;

signals:
    void elastixParametersChanged();

private slots:
    void currentPresetChangedSlot(); ///< Called when the current preset changes. Save to settings and reload dependent values

private:
    void reloadPresets(); ///< Read presets anew and update the current.
    QString getFullParameterFilename(QString filename);
    void addDefaultPreset(QString name, QString executable, QStringList parameterFiles);
    void addDefaultPresets();

    StringDataAdapterXmlPtr mCurrentPreset;
    QString mActiveExecutable;
    QString mActiveParameterFile0;
    QString mActiveParameterFile1;
    XmlOptionFile mOptions;
};
typedef boost::shared_ptr<ElastixParameters> ElastixParametersPtr;

/**
 * @}
 */
} /* namespace cx */

#endif // CXELASTIXPARAMETERS_H
