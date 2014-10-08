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

#ifndef CXTOOLCONFIGUREWIDGET_H_
#define CXTOOLCONFIGUREWIDGET_H_

#include "cxGuiExport.h"

#include <QGroupBox>

#include "cxDefinitions.h"
//#include "cxToolConfigurationParser.h"
#include "cxLegacySingletons.h"
#include "cxTrackerConfiguration.h"

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
  ToolConfigureGroupBox(QWidget* parent = NULL);
  virtual ~ToolConfigureGroupBox();

  void setCurrentlySelectedCofiguration(QString configAbsoluteFilePath);
  QString getCurrenctlySelectedConfiguration() const;

signals:
  void toolSelected(QString absoluteFilePath);

public slots:
  QString requestSaveConfigurationSlot(); ///< will save the currently selected configuration if its been edited
  void setClinicalApplicationSlot(CLINICAL_APPLICATION clinicalApplication);

private slots:
  void configChangedSlot();
  void configEditedSlot();
  void toolsChangedSlot();
  void filterToolsSlot();
  void pathEditedSlot();
  void fileNameEditedSlot();
  void filenameDoneEditingSlot();

private:
  enum state ///< state of a configuration file indicating whether or not its edited
  {
    sEdited = Qt::UserRole
  };

  void populateConfigurations(); ///< populates the combobox with all config files from the current application application
	int addConfigurationToComboBox(QString displayName, QString absoluteFilePath); ///< adds a new configuration file item to the combobox
  void setState(QComboBox* box, int index, bool edited); ///< sets the state of a configuration file to be either edited or not, decides whether to save or not
//  ConfigurationFileParser::Configuration getCurrentConfiguration();
  TrackerConfiguration::Configuration getCurrentConfiguration();
  QString generateConfigName(); ///< generates a name based on the current configuration

  void setState(QLineEdit* line, bool userEdited);

  void populateReference(); ///< populates the ref combobox
  int addRefrenceToComboBox(QString absoluteRefereneFilePath); ///< adds a new tool ref file item to the combobox
  CLINICAL_APPLICATION       mClinicalApplication;

  QComboBox*                mConfigFilesComboBox;
  QLineEdit*                mConfigFilePathLineEdit; ///< path to the folder where the xml should be
  QLineEdit*                mConfigFileLineEdit; ///< name of the xml file (example.xml)
  QComboBox*                mReferenceComboBox;

  SelectionGroupBox*        mApplicationGroupBox;
  SelectionGroupBox*        mTrackingSystemGroupBox;

  ConfigToolListWidget*     mToolListWidget;

};

}

#endif /* CXTOOLCONFIGUREWIDGET_H_ */
