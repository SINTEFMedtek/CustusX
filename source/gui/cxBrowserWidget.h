#ifndef CXBROWSERWIDGET_H_
#define CXBROWSERWIDGET_H_

#include <QWidget>
#include "sscImage.h"

class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;

namespace cx
{
class DataManager;
class ToolManager;
class MessageManager;
class ViewManager;
/**
 * \class BrowserWidget
 *
 * \brief Shows a treestructure containing the loaded images, meshes
 * and tools
 *
 * \date Feb 11, 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class BrowserWidget : public QWidget
{
  Q_OBJECT

public:
  BrowserWidget(QWidget* parent);
  ~BrowserWidget();

signals:
  void currentImageChanged(ssc::ImagePtr currentImage); ///< sends out a signal when the user chooses a different image to work on

protected slots:
  void populateTreeWidget(); ///< fills the tree

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void closeEvent(QCloseEvent* event); ///<disconnects stuff

  //gui
  QTreeWidget* mTreeWidget; ///< the treestructure containing the images, meshes and tools
  QVBoxLayout* mVerticalLayout; ///< vertical layout is used

  //managers
  DataManager* mDataManager; ///< has all the data loaded into the system
  ToolManager* mToolManager; ///< interface to the navigation system
  MessageManager* mMessageManager; ///< takes messages intended for the user
  ViewManager* mViewManager; ///< controls layout of views and has a pool of views

  //data
  ssc::ImagePtr mCurrentImage; ///< the currently selected image

private:
  BrowserWidget();
};
}//end namespace cx

#endif /* CXBROWSERWIDGET_H_ */
