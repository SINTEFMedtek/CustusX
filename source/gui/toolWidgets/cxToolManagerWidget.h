#ifndef CXTOOLMANAGERWIDGET_H_
#define CXTOOLMANAGERWIDGET_H_

#include "cxBaseWidget.h"

namespace cx
{

/**
 * \class ToolManagerWidget
 *
 * \brief Designed as a debugging widget for the cxToolManager
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
};

}

#endif /* CXTOOLMANAGERWIDGET_H_ */
