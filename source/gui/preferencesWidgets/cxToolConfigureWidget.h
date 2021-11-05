/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLCONFIGUREWIDGET_H_
#define CXTOOLCONFIGUREWIDGET_H_

#include "cxGuiExport.h"

#include <QGroupBox>

#include "cxDefinitions.h"
#include "cxTrackerConfiguration.h"
#include "cxStringProperty.h"
#include "cxBoolProperty.h"
#include "cxForwardDeclarations.h"


class QComboBox;
class QLineEdit;

namespace cx
{

class SelectionGroupBox;
class ConfigToolListWidget;

/**
 * \brief Manage tool configurations
 * \ingroup cx_gui
 *
 * A widget for creating/modifying tool configurations. The user
 * can select configs, and set name and which tools that are part
 * of that config.
 *
 * Intended for use with ToolFilterGroupBox, from where the user can
 * drag and drop tools.
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class cxGui_EXPORT ToolConfigureGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  ToolConfigureGroupBox(TrackingServicePtr trackingService, StateServicePtr stateService, QWidget* parent = NULL);
  virtual ~ToolConfigureGroupBox();

  void setCurrentlySelectedCofiguration(QString configAbsoluteFilePath);
  QString getCurrenctlySelectedConfiguration() const;
  StringPropertyBasePtr getTrackingSystemSelector();

signals:
  void toolSelected(QString absoluteFilePath);

public slots:
  QString requestSaveConfigurationSlot(); ///< will save the currently selected configuration if its been edited
  TrackerConfiguration::Configuration getConfiguration();

private slots:
  void configChangedSlot();
  void configEditedSlot();
  void toolsChangedSlot();
  void filterToolsSlot();
  void onApplicationStateChanged();
  void trackingSystemImplementationChangedSlot();
  void applyRefToToolsChangedSlot();

private:
  void createGUISelectors();
  QStringList getTrackingSystemImplementationList();
  void populateConfigurations(); ///< populates the combobox with all config files from the current application application
  int addConfigurationToComboBox(QString displayName, QString absoluteFilePath); ///< adds a new configuration file item to the combobox
  void populateReference(); ///< populates the ref combobox
  int addRefrenceToComboBox(QString absoluteRefereneFilePath); ///< adds a new tool ref file item to the combobox
  QString getCurrentConfigFilePath();

  QComboBox*                mConfigFilesComboBox;
  QLineEdit*                mConfigFileLineEdit; ///< name of the xml file (example.xml)
  QComboBox*                mReferenceComboBox;
  ConfigToolListWidget*     mToolListWidget;
  bool mModified; // if set: content is modified: save on exit
  StringPropertyPtr mTrackingSystemSelector;
  StringPropertyPtr mTrackingSystemImplementationSelector;
  BoolPropertyPtr mApplyRefToTools;
  TrackingServicePtr mTrackingService;
  StateServicePtr mStateService;
};

}

#endif /* CXTOOLCONFIGUREWIDGET_H_ */
