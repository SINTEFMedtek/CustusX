/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXELASTIXPARAMETERS_H
#define CXELASTIXPARAMETERS_H

#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxStringProperty.h"
#include "org_custusx_registration_method_commandline_Export.h"

namespace cx
{
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;

/**
 * \file
 * \addtogroup org_custusx_registration_method_commandline
 * @{
 */

/**
 * \brief Parameter and preset handling for the ElastiX registration.
 *
 * \date Apr 10, 2013
 * \author Christian Askeland, SINTEF
 */
class org_custusx_registration_method_commandline_EXPORT ElastixParameters : public QObject
{
    Q_OBJECT
public:
    ElastixParameters(XmlOptionFile options);
	static QString getConfigUid();
	StringPropertyBasePtr getCurrentPreset();
    void removeCurrentPreset(); ///< Remove the currently selected preset. Reload.
    void saveCurrentPreset(QString newName);

	FilePathPropertyPtr getActiveParameterFile0() const { return mActiveParameterFile0; }
	FilePathPropertyPtr getActiveParameterFile1() const { return mActiveParameterFile1; }
	FilePathPropertyPtr getActiveExecutable() const { return mActiveExecutable; }

    QStringList getActiveParameterFiles() const;
    QString getPresetNameSuggesion() const; ///< create a name describing the active state, can be used as name for a new preset.

signals:
    void elastixParametersChanged();

private slots:
    void currentPresetChangedSlot(); ///< Called when the current preset changes. Save to settings and reload dependent values

private:
    bool validParameterFile(QString file) const;
    void reloadPresets(); ///< Read presets anew and update the current.
    QString getFullParameterFilename(QString filename);
    void addDefaultPreset(QString name, QString executable, QStringList parameterFiles);
    void addDefaultPresets();
	FilePathPropertyPtr getExecutable();
	FilePathPropertyPtr getParameterFile(QString uid);

    StringPropertyPtr mCurrentPreset;
	FilePathPropertyPtr mActiveExecutable;
	FilePathPropertyPtr mActiveParameterFile0;
	FilePathPropertyPtr mActiveParameterFile1;
    XmlOptionFile mOptions;
};
typedef boost::shared_ptr<ElastixParameters> ElastixParametersPtr;

/**
 * @}
 */
} /* namespace cx */

#endif // CXELASTIXPARAMETERS_H
