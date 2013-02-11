#ifndef CXMESHPROPERTIESWIDGET_H_
#define CXMESHPROPERTIESWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscMesh.h"
#include "cxDataInterface.h"

namespace cx
{

/**
 * \class ImagePropertiesWidget
 *
 * \ingroup cxGUI
 *
 * \date 2010.07.07
 * \\author Ole Vegard Solberg, SINTEF
 */
class MeshPropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  MeshPropertiesWidget(QWidget* parent);
  virtual ~MeshPropertiesWidget();

signals:

protected slots:
  void setColorSlot();
  void setColorSlotDelayed();
//  void visibilityChangedSlot(bool visible);
//  void populateMeshComboBoxSlot();
  void meshSelectedSlot();
  void importTransformSlot();
  void deleteDataSlot();
  void backfaceCullingSlot(bool checked);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ssc::MeshPtr mMesh;
//  QComboBox* mMeshComboBox; ///< combobox for selecting mesh
  QGroupBox* mMeshPropertiesGroupBox;
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataNameEditableStringDataAdapterPtr mNameAdapter;
  DataUidEditableStringDataAdapterPtr mUidAdapter;
  SelectMeshStringDataAdapterPtr mSelectMeshWidget;
  QCheckBox* mBackfaceCullingCheckBox;
  
  MeshPropertiesWidget();
};

}//end namespace cx

#endif /* CXMESHPROPERTIESWIDGET_H_ */
