#ifndef CXIMAGEPROPERTIESWIDGET_H_
#define CXIMAGEPROPERTIESWIDGET_H_

#include <vector>
#include <QtGUI>

namespace cx
{

class SliderGroupWidget;

/**
 * \class ImagePropertiesWidget
 *
 *
 * \date 2010.04.12
 * \author: Christian Askeland, SINTEF
 */
class ImagePropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  ImagePropertiesWidget(QWidget* parent);
  virtual ~ImagePropertiesWidget();

signals:

protected slots:
  void updateSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ImagePropertiesWidget();

  SliderGroupWidget* mLevelWidget;
  SliderGroupWidget* mWindowWidget;
  QLabel* mImageNameLabel;
};

}//end namespace cx

#endif /* CXIMAGEPROPERTIESWIDGET_H_ */
