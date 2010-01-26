#ifndef CXPREFERANCESDIALOG_H_
#define CXPREFERANCESDIALOG_H_

#include <QDialog>

class QTabWidget;
class QDialogButtonBox;
class QLabel;
class QComboBox;
class QPushButton;
class QSettings;

namespace cx
{

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

private:
  void setToolConfigFiles();
  void setCurrentToolConfigFile();

  QSettings* mSettings;

  QComboBox* mPatientDataFolderComboBox;
  //QComboBox* mImportDataFolderComboBox;

  QComboBox *mToolConfigFolderComboBox;
  QComboBox *mToolConfigFilesComboBox;

  QString mGlobalPatientDataFolder;
  //QString mCurrentImportDataFolder;
  
  QString mCurrentToolConfigFilePath;
  QString mCurrentToolConfigFolder;
  QString mCurrentToolConfigFile;

};


// TestTab
//------------------------------------------------------------------------------
class TestTab : public QWidget
{
    Q_OBJECT

public:
    TestTab(QWidget *parent = 0);
	~TestTab();
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
  
private:
    FoldersTab *mFoldersTab;
    TestTab *testTab;
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
};


}//namespace cx

#endif
