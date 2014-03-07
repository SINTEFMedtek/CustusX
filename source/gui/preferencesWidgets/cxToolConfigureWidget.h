#ifndef CXTOOLCONFIGUREWIDGET_H_
#define CXTOOLCONFIGUREWIDGET_H_

#include <QGroupBox>

#include "sscDefinitions.h"
#include "cxToolConfigurationParser.h"
#include "cxLegacySingletons.h"

class QComboBox;
class QLineEdit;

namespace cx
{

class SelectionGroupBox;
class ConfigToolListWidget;

/**
 * \brief Manage tool configurations
 * \ingroup cxGUI
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

class ToolConfigureGroupBox : public QGroupBox
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
  int addConfigurationToComboBox(QString displayName, QString absoluteFilePath, bool edited = false); ///< adds a new configuration file item to the combobox
  void setState(QComboBox* box, int index, bool edited); ///< sets the state of a configuration file to be either edited or not, decides whether to save or not
  ConfigurationFileParser::Configuration getCurrentConfiguration();
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
