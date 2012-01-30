/*
 * cxColorSelectButton.cpp
 *
 *  \date May 2, 2011
 *      \author christiana
 */

#include <cxColorSelectButton.h>
#include <QtGui>

namespace cx
{

ColorSelectButton::ColorSelectButton(QString caption, QColor color, QWidget* parent) : QPushButton(caption, parent)
{
  connect(this, SIGNAL(clicked()), this, SLOT(clickedSlot()));
  this->setColor(color);
}

void ColorSelectButton::setColor(QColor color)
{
  mColor = color;

  QPixmap pixmap(16,16);
  pixmap.fill(mColor);
  this->setIcon(QIcon(pixmap));
}

void ColorSelectButton::clickedSlot()
{
  QColor result = QColorDialog::getColor( mColor, this);
  if (!result.isValid())
    return;
  this->setColor(result);
  emit (colorChanged(mColor));
}

}
