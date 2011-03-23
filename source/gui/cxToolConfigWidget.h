#ifndef CXTOOLCONFIGWIDGET_H_
#define CXTOOLCONFIGWIDGET_H_

#include "cxWhatsThisWidget.h"

class QComboBox;
class QDir;

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

private:
  void populateConfigComboBox();
  void populateApplicationFilterComboBox();
  void populateTrackingSystems();
  void populateToolList();
  QStringList getToolFiles(QDir& dir);

  //config files
  QComboBox*      mConfigFilesComboBox;
  QString         mCurrentConfigFile;

  //tracking systems
  QGroupBox*      mTrackingGroup;
  QButtonGroup*   mTrackingButtonGroup;

  //tool files
  QGroupBox*      mToolGroup;
  QComboBox*      mToolFilterComboBox; //filter for shown tools
};

}//namespace cx

#endif /* CXTOOLCONFIGWIDGET_H_ */
