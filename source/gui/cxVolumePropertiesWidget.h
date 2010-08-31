/*
 * cxVolumePropertiesWidget.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"

namespace cx
{


/** Widget that contains a select active image combo box.
 */
class ActiveVolumeWidget : public QWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget() {}
};


/** Widget for displaying and manipulating various 3D Volume properties.
 *
 */
class VolumePropertiesWidget : public QWidget
{
  Q_OBJECT
public:
  VolumePropertiesWidget(QWidget* parent);

};

}//namespace cx


#endif /* CXVOLUMEPROPERTIESWIDGET_H_ */
