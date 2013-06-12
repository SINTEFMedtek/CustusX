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
#ifndef CXELASTIXPARAMETERS_H
#define CXELASTIXPARAMETERS_H

#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscXmlOptionItem.h"
#include "sscStringDataAdapterXml.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
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
    ElastixParameters(ssc::XmlOptionFile options);
    ssc::StringDataAdapterPtr getCurrentPreset();
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
    QString getParameterFilesDir() const;

signals:
    void elastixParametersChanged();

private slots:
    void currentPresetChangedSlot(); ///< Called when the current preset changes. Save to settings and reload dependent values

private:
    void reloadPresets(); ///< Read presets anew and update the current.
    QString getFullParameterFilename(QString filename);
    void addDefaultPreset(QString name, QString executable, QStringList parameterFiles);
    void addDefaultPresets();

    ssc::StringDataAdapterXmlPtr mCurrentPreset;
    QString mActiveExecutable;
    QString mActiveParameterFile0;
    QString mActiveParameterFile1;
    ssc::XmlOptionFile mOptions;
};
typedef boost::shared_ptr<ElastixParameters> ElastixParametersPtr;

/**
 * @}
 */
} /* namespace cx */

#endif // CXELASTIXPARAMETERS_H
