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
  ToolConfigureGroupBox(ssc::MEDICAL_DOMAIN medicalDomain, QWidget* parent = NULL);
  virtual ~ToolConfigureGroupBox();

signals:
  void toolSelected(QString absoluteFilePath);

public slots:
  void requestSaveConfigurationSlot(); ///< will save the currently selected configuration if its been edited

private slots:
  void configChangedSlot();
  void configEditedSlot();
  void toolsChangedSlot();
  void filterToolsSlot();

private:
  enum state ///< state of a configuration file indicating whether or not its edited
  {
    sEdited = Qt::UserRole
  };

  void populateConfigurations(); ///< populates the combobox with all config files from the current application domain
  int addConfigurationToComboBox(QString displayName, QString absoluteFilePath, bool edited = false); ///< adds a new configuration file item to the combobox
  void setState(QComboBox* box, int index, bool edited); ///< sets the state of a configuration file to be either edited or not, decides whether to save or not
  ConfigurationFileParser::Configuration getCurrentConfiguration();
  QString generateConfigName(); ///< generates a name based on the current configuration

  void populateReference(); ///< populates the ref combobox
  int addRefrenceToComboBox(QString absoluteRefereneFilePath); ///< adds a new tool ref file item to the combobox

  ssc::MEDICAL_DOMAIN       mMedicalDomain;

  QComboBox*                mConfigFilesComboBox;
  QLineEdit*                mConfigFilePathLineEdit;
  QComboBox*                mReferenceComboBox;

  SelectionGroupBox*        mApplicationGroupBox;
  SelectionGroupBox*        mTrackingSystemGroupBox;

  ConfigToolListWidget*     mToolListWidget;

};

}

#endif /* CXTOOLCONFIGUREWIDGET_H_ */
