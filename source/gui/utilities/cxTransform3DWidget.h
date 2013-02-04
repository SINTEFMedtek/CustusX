/*
 * cxAffineMatrixWidget.h
 *
 *  \date Mar 19, 2011
 *      \author christiana
 */

#ifndef CXTRANSFORM3DWIDGET_H_
#define CXTRANSFORM3DWIDGET_H_

#include "cxBaseWidget.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscTransform3D.h"
#include "sscFrame3D.h"

class QTextEdit;
class QVBoxLayout;
class QAction;
class QFrame;

namespace cx
{

class MatrixTextEdit;

/**
 * \brief Widget for displaying and manipulating an affine matrix,
 * i.e. a rotation+translation matrix.
 * \ingroup cxGUI
 *
 */
class Transform3DWidget : public BaseWidget
{
  Q_OBJECT
public:
  Transform3DWidget(QWidget* parent = NULL);
  virtual ~Transform3DWidget();
  virtual QString defaultWhatsThis() const;

  void setMatrix(const ssc::Transform3D& M);
  ssc::Transform3D getMatrix() const;
  void setEditable(bool edit);

signals:
  void changed();

private slots:
  void changedSlot();
  void toggleEditSlot();
  void textEditChangedSlot();

private:
  void updateValues();
  void addAngleControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void addTranslationControls(QString uid, QString name, int index, QVBoxLayout* layout);
  void rotateSlot(QPointF delta, int index);
  void translateSlot(QPointF delta, int index);
  void updateAdapter(ssc::DoubleDataAdapterXmlPtr adapter, double value);

  ssc::Frame3D mFrame;
  MatrixTextEdit* mTextEdit;
  boost::array<ssc::DoubleDataAdapterXmlPtr, 3> mAngleAdapter;
  boost::array<ssc::DoubleDataAdapterXmlPtr, 3> mTranslationAdapter;
  bool recursive;
  bool mBlockChanges;
  QAction* mEditAction;
  QFrame* aGroupBox;
  QFrame* tGroupBox;
  ssc::DecomposedTransform3D mDecomposition;

//  //  Vector3DWidget(QWidget* parent, ssc::Vector3DDataAdapterPtr data)
//    Vector3DDataAdapterPtr mAngleAdapter;
//    Vector3DDataAdapterPtr mTranslationAdapter;

//  template<class T>
//  QAction* createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot);
};

}

#endif /* CXTRANSFORM3DWIDGET_H_ */
