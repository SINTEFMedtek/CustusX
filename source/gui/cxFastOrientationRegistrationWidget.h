#ifndef CXFASTORIENTATIONREGISTRATIONWIDGET_H_
#define CXFASTORIENTATIONREGISTRATIONWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"

class QPushButton;

namespace cx
{
/**
 * \class FastOrientationRegistrationWidget
 *
 * \brief Widget for registrating the orientation part of a fast registration
 *
 * \date 27. sep. 2010
 * \author: Janne Beate Bakeng
 */

class FastOrientationRegistrationWidget : public QWidget
{
  Q_OBJECT

public:
  FastOrientationRegistrationWidget(QWidget* parent);
  ~FastOrientationRegistrationWidget();

protected:
  virtual void showEvent(QShowEvent* event);
  virtual void hideEvent(QHideEvent* event);

private slots:
  void setOrientationSlot();
  void dominantToolChangedSlot(const std::string& uid);
  void enableToolSampleButtonSlot();
  void toolVisibleSlot(bool visible);

private:
  QPushButton* mSetOrientationButton;
  ssc::ToolPtr mToolToSample; ///< tool to be sampled from
};
}
#endif /* CXFASTORIENTATIONREGISTRATIONWIDGET_H_ */
