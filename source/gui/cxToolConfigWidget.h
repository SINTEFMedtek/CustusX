#ifndef CXTOOLCONFIGWIDGET_H_
#define CXTOOLCONFIGWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "cxTool.h"

class QComboBox;
class QDir;
class QListWidget;

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

private slots:
  void applicationStateChangedSlot();
  void filterToolsSlot();

private:
  void populateConfigComboBox();
  void populateApplicationFilter();
  void populateTrackingSystems();
  void populateToolList(QStringList applicationFilter = QStringList(), QStringList trackingSystemFilter = QStringList());

  QStringList getToolFiles(QDir& dir);
  QList<Tool::InternalStructure> getToolInternals(QStringList toolAbsoluteFilePaths);
  Tool::InternalStructure getToolInternal(QString toolAbsoluteFilePath);

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
  QGroupBox*      mApplicationFilterGroupBox;
  QButtonGroup*   mApplicationFilterButtonGroup;
};

}//namespace cx

#endif /* CXTOOLCONFIGWIDGET_H_ */
