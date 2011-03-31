/*
 * cxAffineMatrixWidget.h
 *
 *  Created on: Mar 19, 2011
 *      Author: christiana
 */

#ifndef CXTRANSFORM3DWIDGET_H_
#define CXTRANSFORM3DWIDGET_H_

#include <QWidget>
class QTextEdit;
class QVBoxLayout;
class QAction;
class QFrame;
#include "sscDoubleDataAdapterXml.h"
#include "sscTransform3D.h"
#include "sscFrame3D.h"

namespace cx
{


class MatrixTextEdit;

/**Widget for displaying and manipulating an affine matrix,
 * i.e. a rotation+translation matrix.
 *
 */
class Transform3DWidget : public QWidget
{
  Q_OBJECT
public:
  Transform3DWidget(QWidget* parent = NULL);
  virtual ~Transform3DWidget();

  void setMatrix(const ssc::Transform3D& M);
  ssc::Transform3D getMatrix() const;

signals:
  void changed();

private slots:
  void changedSlot();
  void toggleEditSlot();
  void textEditChangedSlot();

private:
  void updateValues();
//  ssc::Transform3D mMatrix;
  ssc::Frame3D mFrame;
  void addAngleControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void addTranslationControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void rotateSlot(QPointF delta, int index);
  void translateSlot(QPointF delta, int index);
  void setEditable(bool edit);

  MatrixTextEdit* mTextEdit;
  boost::array<ssc::DoubleDataAdapterXmlPtr, 3> mAngleAdapter;
  boost::array<ssc::DoubleDataAdapterXmlPtr, 3> mTranslationAdapter;
  bool recursive;
  QAction* mEditAction;
  QFrame* aGroupBox;
  QFrame* tGroupBox;
  ssc::DecomposedTransform3D mDecomposition;

  template<class T>
  QAction* createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot);
};

}

#endif /* CXTRANSFORM3DWIDGET_H_ */
