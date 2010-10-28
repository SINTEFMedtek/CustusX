#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"

namespace cx
{
/**
 * \class ActiveVolumeWidget
 * \brief Widget that contains a select active image combo box.
 * \date Aug 20, 2010
 * \author christiana
 */
class ActiveVolumeWidget : public QWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget() {}
};

/**
 * \class VolumePropertiesWidget
 * \brief Widget for displaying and manipulating various 3D Volume properties.
 * \date Aug 20, 2010
 * \author christiana
 */
class VolumePropertiesWidget : public QWidget
{
  Q_OBJECT
public:
  VolumePropertiesWidget(QWidget* parent);

};

}//namespace cx


#endif /* CXVOLUMEPROPERTIESWIDGET_H_ */
