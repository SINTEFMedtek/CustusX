/*
 * cxColorSelectButton.h
 *
 *  Created on: May 2, 2011
 *      Author: christiana
 */

#ifndef CXCOLORSELECTBUTTON_H_
#define CXCOLORSELECTBUTTON_H_

#include <QPushButton>
#include <QColor>

namespace cx
{

/**
 * \brief Convenience button for chosing color.
 * \ingroup cxGUI
 * Will open a color select dialog, and color
 * the button itself in the selected color.
 *
 */
class ColorSelectButton : public QPushButton
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


}

#endif /* CXCOLORSELECTBUTTON_H_ */
