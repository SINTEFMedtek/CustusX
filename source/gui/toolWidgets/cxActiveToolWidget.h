#ifndef CXACTIVETOOLWIDGET_H_
#define CXACTIVETOOLWIDGET_H_

#include "cxBaseWidget.h"

namespace cx
{
/**
 * \class ActiveToolWidget
 *
 * \brief Widget that contains a select active tool combo box
 * \ingroup cx_gui
 *
 * \date May 4, 2011
 * \author Christian Askeland, SINTEF
 */
class ActiveToolWidget : public BaseWidget
{
  Q_OBJECT
public:
  ActiveToolWidget(QWidget* parent);
  virtual ~ActiveToolWidget() {}

  virtual QString defaultWhatsThis() const;
};

}

#endif /* CXACTIVETOOLWIDGET_H_ */
