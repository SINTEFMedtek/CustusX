/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTRANSFORM3DWIDGET_H_
#define CXTRANSFORM3DWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "cxTransform3D.h"
#include "cxFrame3D.h"

class QTextEdit;
class QVBoxLayout;
class QAction;
class QFrame;

namespace cx
{

class MatrixTextEdit;
typedef boost::shared_ptr<class DoubleProperty> DoublePropertyPtr;

/**
 * \brief Widget for displaying and manipulating an affine matrix,
 * i.e. a rotation+translation matrix.
 *
 * \ingroup cx_resource_widgets
 *
 */
class cxResourceWidgets_EXPORT Transform3DWidget : public BaseWidget
{
  Q_OBJECT
public:
  Transform3DWidget(QWidget* parent = NULL);
  virtual ~Transform3DWidget();

  void setMatrix(const Transform3D& M);
  Transform3D getMatrix() const;
  void setEditable(bool edit);

signals:
  void changed();

protected slots:
  virtual void prePaintEvent();
private slots:
  void changedSlot();
  void toggleEditSlot();
  void textEditChangedSlot();
  void toggleInvertSlot();

private:
  void setMatrixInternal(const Transform3D& M);
  Transform3D getMatrixInternal() const;

  void addAngleControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void addTranslationControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void rotateSlot(QPointF delta, int index);
  void translateSlot(QPointF delta, int index);
  void updateAdapter(DoublePropertyPtr adapter, double value);
  void setActionText(QAction* action, QString text, QString tip);
  Transform3D convertToFromExternal(const Transform3D& M) const;
  void updateInvertAction();

//  Frame3D mFrame;
  MatrixTextEdit* mTextEdit;
  boost::array<DoublePropertyPtr, 3> mAngleAdapter;
  boost::array<DoublePropertyPtr, 3> mTranslationAdapter;
  bool recursive;
  bool mBlockChanges;
  QAction* mEditAction;
  QAction* mInvertAction;
  QFrame* aGroupBox;
  QFrame* tGroupBox;
  DecomposedTransform3D mDecomposition;
};

}

#endif /* CXTRANSFORM3DWIDGET_H_ */
