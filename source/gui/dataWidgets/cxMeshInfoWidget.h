// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXMESHINFOWIDGET_H_
#define CXMESHINFOWIDGET_H_

#include <vector>
#include <QtGui>
#include "cxMesh.h"
#include "cxDataInterface.h"
#include "cxInfoWidget.h"

namespace cx
{
typedef boost::shared_ptr<class SelectMeshStringDataAdapter> SelectMeshStringDataAdapterPtr;

/**
 * \class MeshInfoWidget
 *
 * \brief Widget for displaying information about meshes.
 *
 * \ingroup cx_gui
 *
 * \date 2010.07.07
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
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
  void meshChangedSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  void addWidgets();

  MeshPtr mMesh;
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataNameEditableStringDataAdapterPtr mNameAdapter;
  DataUidEditableStringDataAdapterPtr mUidAdapter;
  SelectMeshStringDataAdapterPtr mSelectMeshWidget;
  ColorDataAdapterXmlPtr mColorAdapter;
  QCheckBox* mBackfaceCullingCheckBox;
  QCheckBox* mFrontfaceCullingCheckBox;
  
  MeshInfoWidget();
};

}//end namespace cx

#endif /* CXMESHINFOWIDGET_H_ */
