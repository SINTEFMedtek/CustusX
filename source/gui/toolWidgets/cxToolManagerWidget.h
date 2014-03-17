#ifndef CXTOOLMANAGERWIDGET_H_
#define CXTOOLMANAGERWIDGET_H_

#include "cxBaseWidget.h"
#include "cxLegacySingletons.h"

class QPushButton;

namespace cx
{

/**
 * \class ToolManagerWidget
 *
 * \brief Designed as a debugging widget for the cxToolManager
 * \ingroup cx_gui
 *
 * \date May 25, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolManagerWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolManagerWidget(QWidget* parent = NULL);
  virtual ~ToolManagerWidget();

  virtual QString defaultWhatsThis() const;

private slots:
  void configureClickedSlot(bool);
  void deconfigureClickedSlot(bool);
  void initializeClickedSlot(bool);
  void uninitializeClickedSlot(bool);
  void startTrackingClickedSlot(bool);
  void stopTrackingClickedSlot(bool);

  void updateButtonStatusSlot(); ///< makes sure that the buttons represent the status of the toolmanager

private:
  QPushButton* mConfigureButton;
  QPushButton* mDeConfigureButton;
  QPushButton* mInitializeButton;
  QPushButton* mUnInitializeButton;
  QPushButton* mStartTrackingButton;
  QPushButton* mStopTrackingButton;
};

}

#endif /* CXTOOLMANAGERWIDGET_H_ */
