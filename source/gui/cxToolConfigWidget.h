#ifndef CXTOOLCONFIGWIDGET_H_
#define CXTOOLCONFIGWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "cxTool.h"

class QComboBox;
class QDir;
class QListWidget;
class QListWidgetItem;

namespace cx
{
/**
 *\brief ToolConfigWidget
 *
 * \brief Widget for setting up a tool configuration for Cx3
 *
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class ToolConfigWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ToolConfigWidget(QWidget* parent);
  virtual ~ToolConfigWidget();

  virtual QString defaultWhatsThis() const;

  QString getSelectedFile() const;

signals:
  void toolSelected(QString absoluteFilePath);

private slots:
  void applicationStateChangedSlot();
  void filterToolsSlot();
  void configChangedSlot();
  void toolClickedSlot(QListWidgetItem* item);
  void toolDoubleClickedSlot(QListWidgetItem* item);
  void fileSelectedSlot(QString fileSelected);

private:
  void populateConfigComboBox();
  void populateApplicationFilter();
  void populateTrackingSystems();
  void populateToolList(QStringList applicationFilter = QStringList(), QStringList trackingSystemFilter = QStringList(), QStringList absoluteToolFilePathsFilter = QStringList());

  void filterButtonGroup(QButtonGroup* group, QStringList filter = QStringList());

  QStringList getToolFiles(QDir& dir);
  QList<Tool::InternalStructure> getToolInternals(QStringList toolAbsoluteFilePaths);
  Tool::InternalStructure getToolInternal(QString toolAbsoluteFilePath);

  QString         mCurrentlySelectedFile;

  //config files
  QComboBox*      mConfigFilesComboBox;
  QString         mCurrentConfigFile;

  //tracking systems
  QGroupBox*      mTrackerGroupBox;
  QButtonGroup*   mTrackerButtonGroup;

  //tool files
  QGroupBox*      mToolGroup;
  QListWidget*    mToolListWidget;

  //applications
  QGroupBox*      mApplicationGroupBox;
  QButtonGroup*   mApplicationButtonGroup;
};

}//namespace cx

#endif /* CXTOOLCONFIGWIDGET_H_ */
