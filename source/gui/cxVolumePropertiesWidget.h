#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

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

/**Widget for displaying various volume information.
 * Part of the VolumePropertiesWidget.
 */
class VolumeInfoWidget : public QWidget
{
  Q_OBJECT

public:
  VolumeInfoWidget(QWidget* parent);
  virtual ~VolumeInfoWidget();

signals:

protected slots:
  void updateSlot();
//  void setColorSlot();
//  void setColorSlotDelayed();
//  void visibilityChangedSlot(bool visible);
//  void populateMeshComboBoxSlot();
//  void meshSelectedSlot(const QString& comboBoxText);
//  void importTransformSlot();
  void deleteDataSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
//  ssc::MeshPtr mMesh;
//  QComboBox* mMeshComboBox; ///< combobox for selecting mesh
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataNameEditableStringDataAdapterPtr mNameAdapter;
  DataUidEditableStringDataAdapterPtr mUidAdapter;
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
