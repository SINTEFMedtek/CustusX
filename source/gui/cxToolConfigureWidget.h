#ifndef CXTOOLCONFIGUREWIDGET_H_
#define CXTOOLCONFIGUREWIDGET_H_

#include <QGroupBox>

#include "sscDefinitions.h"
#include "cxToolConfigurationParser.h"

class QComboBox;
class QLineEdit;

namespace cx
{

class SelectionGroupBox;
class ConfigToolListWidget;

/**
 * ToolConfigureWidget
 *
 * \brief
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
  void setClinicalApplicationSlot(ssc::CLINICAL_APPLICATION clinicalApplication);

private slots:
  void configChangedSlot();
  void configEditedSlot();
  void toolsChangedSlot();
  void filterToolsSlot();
  void pathEditedSlot();

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

  ssc::CLINICAL_APPLICATION       mClinicalApplication;

  QComboBox*                mConfigFilesComboBox;
  QLineEdit*                mConfigFilePathLineEdit;
  QComboBox*                mReferenceComboBox;

  SelectionGroupBox*        mApplicationGroupBox;
  SelectionGroupBox*        mTrackingSystemGroupBox;

  ConfigToolListWidget*     mToolListWidget;

};

}

#endif /* CXTOOLCONFIGUREWIDGET_H_ */
