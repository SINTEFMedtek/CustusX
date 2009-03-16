#ifndef CXCONTEXTDOCKWIDGET_H_
#define CXCONTEXTDOCKWIDGET_H_

#include <QDockWidget>

#include "sscImage.h"

/**
 * cxContextDockWidget.h
 *
 * \brief
 *
 * \date Mar 16, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */
class QTabWidget;
class QVBoxLayout;
class QComboBox;

namespace ssc
{
class DataManager;
}
namespace cx
{
typedef ssc::DataManager DataManager;

class RegistrationManager;
class ToolManager;
class MessageManager;
class ViewManager;
class RepManager;

class ContextDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  ContextDockWidget();
  ~ContextDockWidget();

  int addTab(QWidget * page, const QString & label); ///<
  void removeTab(int tabIndex); ///<

signals:
  void currentImageChanged(ssc::ImagePtr currentImage); ///<

protected slots:
  void visibilityOfDockWidgetChangedSlot(bool visible); ///<
  void populateTheImageComboBoxSlot(); ///<
  void imageSelectedSlot(const QString& comboBoxText); ///<

protected:

  QWidget* mGuiWidget; ///<
  QVBoxLayout* mVerticalLayout; ///<
  QComboBox* mImagesComboBox; ///<
  QTabWidget* mTabWidget; ///<

  ssc::ImagePtr mCurrentImage; ///<

  DataManager* mDataManager; ///<
  RegistrationManager* mRegistrationManager; ///<
  ToolManager* mToolManager; ///<
  MessageManager* mMessageManager; ///<
  ViewManager* mViewManager; ///<
  RepManager* mRepManager; ///<
};
}//namespace cx
#endif /* CXCONTEXTDOCKWIDGET_H_ */
