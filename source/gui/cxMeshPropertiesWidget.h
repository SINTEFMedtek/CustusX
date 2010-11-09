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
 *
 * \date 2010.07.07
 * \author: Ole Vegard Solberg, SINTEF
 */
class MeshPropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  MeshPropertiesWidget(QWidget* parent);
  virtual ~MeshPropertiesWidget();

signals:

protected slots:
  void updateSlot();
  void setColorSlot();
  void setColorSlotDelayed();
  void visibilityChangedSlot(bool visible);
  void populateMeshComboBoxSlot();
  void meshSelectedSlot(const QString& comboBoxText);
  void importTransformSlot();
  void deleteDataSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ssc::MeshPtr mMesh;
  QComboBox* mMeshComboBox; ///< combobox for selecting mesh
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  
  MeshPropertiesWidget();
};

}//end namespace cx

#endif /* CXMESHPROPERTIESWIDGET_H_ */
