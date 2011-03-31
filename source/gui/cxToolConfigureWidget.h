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

public slots:
  void requestSaveConfigurationSlot(); ///< will save the currently selected configuration if its been edited

private slots:
  void configChangedSlot();
  void configEditedSlot();
  void toolsChangedSlot();

private:
  enum state ///< state of a configuration file indicating whether or not its edited
  {
    sEdited = Qt::UserRole
  };

  void populateConfigurations();
  int addConfigurationToComboBox(QString displayName, QString absoluteFilePath, bool edited = false);
  void setState(QComboBox* box, int index, bool edited);
  ConfigurationFileParser::Configuration getCurrentConfiguration();

  void populateReference();
  int addRefrenceToComboBox(QString absoluteRefereneFilePath);

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
