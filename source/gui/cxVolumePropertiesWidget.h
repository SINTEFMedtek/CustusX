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
class QComboBox;

namespace cx
{

/**
 */
class ActiveVolumeWidget : public QWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget();

signals:
  void changeTabIndex(int index); ///< Send a signal when the tab index must be changed

protected slots:
  void populateTheImageComboBoxSlot(); ///< populates the combobox with the images loaded into the datamanager
  void imageSelectedSlot(const QString& comboBoxText); ///< sends out a signal and adds reps of the image to the views
  void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed

protected:
  QComboBox* mImagesComboBox; ///< combobox for displaying available images
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
