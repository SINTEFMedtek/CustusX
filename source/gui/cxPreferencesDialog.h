#ifndef CXPREFERANCESDIALOG_H_
#define CXPREFERANCESDIALOG_H_

#include <QDialog>
#include "boost/shared_ptr.hpp"
#include "sscDoubleDataAdapterXml.h"

class QTabWidget;
class QDialogButtonBox;
class QLabel;
class QComboBox;
class QPushButton;
class QSettings;
class QSpinBox;
class QCheckBox;
class QGridLayout;
class QVBoxLayout;
class QLineEdit;
typedef boost::shared_ptr<class QSettings> QSettingsPtr;

namespace cx
{
class MessageManager;
class ViewManager;

/**
 * \class FoldersTab
 *
 * \brief Configure default folder in preferences dialog
 *
 * \date Jan 25, 2010
 * \author Frank Lindseth, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class FoldersTab : public QWidget
{
  Q_OBJECT

public:
  FoldersTab(QWidget *parent = 0);
  virtual ~FoldersTab();
  void init();

public slots:
  void saveParametersSlot();

signals:
  void savedParameters();
  
private slots:
  void browsePatientDataFolderSlot();

  void currentToolConfigFilesIndexChangedSlot(const QString & newToolConfigFile);
  void currenApplicationChangedSlot(int index);
  void applicationStateChangedSlot();

private:
  void setToolConfigComboBox();
  void setApplicationComboBox();

  QSettingsPtr mSettings;

  QComboBox* mPatientDataFolderComboBox;

  QComboBox* mToolConfigFolderComboBox;
  QComboBox* mToolConfigFilesComboBox;
  QComboBox* mChooseApplicationComboBox;

  QString mGlobalPatientDataFolder;
  
  QString mCurrentToolConfigFile;

};

/**
 * \class PerformanceTab
 *
 * \brief Configure performance tab in preferences dialog
 *
 * \date Mar 8, 2010
 * \author Ole Vegard Solberg, SINTEF
 */
class PerformanceTab : public QWidget
{
    Q_OBJECT

public:
  PerformanceTab(QWidget *parent = 0);
  void init();
  
  public slots:
  void saveParametersSlot();
  
signals:
  void renderingIntervalChanged(int);
  void shadingChanged(bool);

protected:
  QSettingsPtr mSettings;
  QSpinBox* mRenderingIntervalSpinBox;
  QLabel* mRenderingRateLabel;
  QCheckBox* mSmartRenderCheckBox;
  QCheckBox* mGPURenderCheckBox;
  QCheckBox* mShadingCheckBox;
  QGridLayout *mMainLayout;
  ssc::DoubleDataAdapterXmlPtr mMaxRenderSize;

  private slots:
  void renderingIntervalSlot(int interval);
  void setBackgroundColorSlot();
};

/**
 * \class AutomationTab
 *
 * \brief Automatic execution of actions when applicable
 *
 * \date Jan 27, 2011
 * \author Christian Askeland, SINTEF
 */
class AutomationTab : public QWidget
{
  Q_OBJECT

public:
  AutomationTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();


protected:
  QSettingsPtr mSettings;
  QCheckBox* mAutoStartTrackingCheckBox;
  QCheckBox* mAutoStartStreamingCheckBox;
  QCheckBox* mAutoReconstructCheckBox;
  QVBoxLayout *mMainLayout;
};

/**
 * \class UltrasoundTab
 *
 * \brief Various parameters related to ultrasound acquisition and reconstruction.
 *
 * \date Jan 27, 2011
 * \author Christian Askeland, SINTEF
 */
class UltrasoundTab : public QWidget
{
  Q_OBJECT

public:
  UltrasoundTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();


protected:
  QSettingsPtr mSettings;
  QLineEdit* mAcquisitionNameLineEdit;
  QVBoxLayout *mMainLayout;
};

/**
 * \class PreferencesDialog
 *
 * \brief Set application preferences
 *
 * \author Frank Lindseth, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
  PreferencesDialog(QWidget *parent = 0);
  virtual ~PreferencesDialog();

protected:
  ViewManager* mViewManager; ///< controls layout of views and has a pool of views
  FoldersTab *mFoldersTab;
  PerformanceTab *mPerformanceTab;
  AutomationTab* mAutomationTab;
  UltrasoundTab* mUltrasoundTab;
  QTabWidget *tabWidget;
  QDialogButtonBox *buttonBox;
};



}//namespace cx

#endif
