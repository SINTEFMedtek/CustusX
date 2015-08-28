/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
