/*
 * cxAffineMatrixWidget.h
 *
 *  Created on: Mar 19, 2011
 *      Author: christiana
 */

#ifndef CXAFFINEMATRIXWIDGET_H_
#define CXAFFINEMATRIXWIDGET_H_

#include <QWidget>
class QTextEdit;
class QVBoxLayout;
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
class AffineMatrixWidget : public QWidget
{
  Q_OBJECT
public:
  AffineMatrixWidget(QWidget* parent = NULL);
  virtual ~AffineMatrixWidget();

  void setMatrix(const ssc::Transform3D& M);
  ssc::Transform3D getMatrix() const;

signals:
  void changed();

private slots:
  void changedSlot();

private:
  void updateValues();
//  ssc::Transform3D mMatrix;
  ssc::Frame3D mFrame;
  void addAngleControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void rotateSlot(QPointF delta, int index);

  MatrixTextEdit* mTextEdit;
  boost::array<ssc::DoubleDataAdapterXmlPtr, 3> mAngleAdapter;
//  ssc::DoubleDataAdapterXmlPtr mXAngle;
//  ssc::DoubleDataAdapterXmlPtr mYAngle;
//  ssc::DoubleDataAdapterXmlPtr mZAngle;
  bool recursive;
};

}

#endif /* CXAFFINEMATRIXWIDGET_H_ */
