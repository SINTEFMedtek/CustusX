/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCOLORSELECTBUTTON_H_
#define CXCOLORSELECTBUTTON_H_

#include "cxResourceWidgetsExport.h"

#include <QToolButton>
#include <QColor>

namespace cx
{

/**
 * \brief Convenience button for chosing color.
 * Will open a color select dialog, and color
 * the button itself in the selected color.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT ColorSelectButton : public QToolButton
{
  Q_OBJECT
public:
  ColorSelectButton(QString caption, QColor color=Qt::white, QWidget* parent=NULL);
  void setColor(QColor color);
signals:
  void colorChanged(QColor);

private:
  QColor mColor;
private slots:
  void clickedSlot();
};
//------------------------------------------------------------------------------

/**
 * Helper class for setting a color in settings()
 */
class cxResourceWidgets_EXPORT SelectColorSettingButton : public ColorSelectButton
{
	Q_OBJECT

public:
	SelectColorSettingButton(QString caption, QString id, QString help);
	void storeColor(QColor color);

private:
	QString mId;
};


}

#endif /* CXCOLORSELECTBUTTON_H_ */
