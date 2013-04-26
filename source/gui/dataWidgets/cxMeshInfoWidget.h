#ifndef CXMESHINFOWIDGET_H_
#define CXMESHINFOWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscMesh.h"
#include "cxDataInterface.h"
#include "cxInfoWidget.h"

namespace cx
{
typedef boost::shared_ptr<class SelectMeshStringDataAdapter> SelectMeshStringDataAdapterPtr;

/**
 * \class ImagePropertiesWidget
 *
 * \ingroup cxGUI
 *
 * \date 2010.07.07
 * \author Ole Vegard Solberg, SINTEF
 */
class MeshInfoWidget : public InfoWidget
{
  Q_OBJECT

public:
  MeshInfoWidget(QWidget* parent);
  virtual ~MeshInfoWidget();

protected slots:
  void setColorSlot();
  void setColorSlotDelayed();
  void meshSelectedSlot();
  void importTransformSlot();
//  void deleteDataSlot();
  void meshChangedSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  void addWidgets();

  ssc::MeshPtr mMesh;
//  QGroupBox* mMeshPropertiesGroupBox;
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataNameEditableStringDataAdapterPtr mNameAdapter;
  DataUidEditableStringDataAdapterPtr mUidAdapter;
  SelectMeshStringDataAdapterPtr mSelectMeshWidget;
  ssc::ColorDataAdapterXmlPtr mColorAdapter;
  QCheckBox* mBackfaceCullingCheckBox;
  QCheckBox* mFrontfaceCullingCheckBox;
  
  MeshInfoWidget();
};

}//end namespace cx

#endif /* CXMESHINFOWIDGET_H_ */
