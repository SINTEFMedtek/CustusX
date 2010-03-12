#ifndef CXPREFERANCESDIALOG_H_
#define CXPREFERANCESDIALOG_H_

#include <QDialog>

class QTabWidget;
class QDialogButtonBox;
class QLabel;
class QComboBox;
class QPushButton;
class QSettings;
class QSpinBox;
class QCheckBox;
class QGridLayout;

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
  ~FoldersTab();
  void init();

public slots:
  void saveParametersSlot();

signals:
  void savedParameters();
  
private slots:
  void browsePatientDataFolderSlot();
  //void browseImportDataFolderSlot();

  void browseToolConfigFolderSlot();
  void currentToolConfigFilesIndexChangedSlot(const QString & newToolConfigFile);
  void currenApplicationChangedSlot(const QString & newApplicationName);

private:
  void setToolConfigFiles();
  void setApplicationComboBox();
  void setCurrentToolConfigFile();
  void setCurrentApplication();

  QSettings* mSettings;

  QComboBox* mPatientDataFolderComboBox;
  //QComboBox* mImportDataFolderComboBox;

  QComboBox* mToolConfigFolderComboBox;
  QComboBox* mToolConfigFilesComboBox;
  QComboBox* mChooseApplicationComboBox;

  QString mGlobalPatientDataFolder;
  //QString mCurrentImportDataFolder;
  
  QString mCurrentToolConfigFilePath;
  QString mCurrentToolConfigFolder;
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
  QSettings* mSettings;
  QSpinBox* mRenderingIntervalSpinBox;
  QLabel* mRenderingRateLabel;
  QCheckBox* mShadingCheckBox;
  QGridLayout *mMainLayout;
  
  private slots:
  void renderingIntervalSlot(int interval);
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
  ~PreferencesDialog();	
  
protected:
  ViewManager* mViewManager; ///< controls layout of views and has a pool of views
  FoldersTab *mFoldersTab;
  PerformanceTab *mPerformanceTab;
  QTabWidget *tabWidget;
  QDialogButtonBox *buttonBox;
};


}//namespace cx

#endif
