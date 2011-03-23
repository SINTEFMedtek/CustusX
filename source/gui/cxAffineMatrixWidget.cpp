/*
 * cxAffineMatrixWidget.cpp
 *
 *  Created on: Mar 19, 2011
 *      Author: christiana
 */

#include <cxAffineMatrixWidget.h>

#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QFontMetrics>

#include "sscTypeConversions.h"

#include "sscDoubleDataAdapterXml.h"
#include "sscDoubleWidgets.h"
#include <cmath>
#include "cxCameraControlWidget.h"

#include "boost/bind.hpp"
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"

namespace cx
{

class MatrixTextEdit : public QTextEdit
{
public:
  MatrixTextEdit(QWidget* parent=NULL) : QTextEdit(parent)  {}
  QSize minimumSizeHint() const { return sizeHint(); }
  QSize sizeHint() const
  {
//    QRect rect2 = QFontMetrics(this->font()).boundingRect("M");
//    std::cout << "text2" << " M" << " wh= " << rect2.width() << " " << rect2.height() << std::endl;
    QString text = qstring_cast(ssc::Transform3D()).split("\n")[0];
    QRect rect = QFontMetrics(this->font()).boundingRect(text);
//    QSize sz = QFontMetrics(this->font()).size(0, qstring_cast(ssc::Transform3D()));
//    std::cout << "sizesz " << sz.width() << " " << sz.height() << std::endl;
//    std::cout << "text" << text << " wh= " << rect.width() << " " << rect.height() << std::endl;
    QSize s(rect.width()*1.1+5, 4*rect.height()*1.1+5);
//    std::cout << "size " << s.width() << " " << s.height() << std::endl;
    return s;
//    return QSize(1,1);
  }
};

AffineMatrixWidget::AffineMatrixWidget(QWidget* parent) :
    QWidget(parent)
{
  recursive = false;
  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
//  mGroup = new QFrame;
//  QHBoxLayout* topLayout = new QHBoxLayout;
//  toptopLayout->addWidget(mGroup);
//  mGroup->setLayout(topLayout);

  mTextEdit = new MatrixTextEdit;
//  mTextEdit->setVisible(false);
  mTextEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Maximum);
  mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
  mTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  toptopLayout->addWidget(mTextEdit);

  this->addAngleControls("xAngle", "X Angle", 0, toptopLayout);
  this->addAngleControls("yAngle", "Y Angle", 1, toptopLayout);
  this->addAngleControls("zAngle", "Z Angle", 2, toptopLayout);

//  mXAngle = ssc::DoubleDataAdapterXml::initialize("xAngle", "X Angle", "", 0, ssc::DoubleRange(-M_PI,M_PI,M_PI/180),0);
//  connect(mXAngle.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
//  mXAngle->setInternal2Display(180/M_PI);
//
//  mYAngle = ssc::DoubleDataAdapterXml::initialize("yAngle", "Y Angle", "", 0, ssc::DoubleRange(-M_PI,M_PI,M_PI/180),0);
//  connect(mYAngle.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
//  mYAngle->setInternal2Display(180/M_PI);
//
//  mZAngle = ssc::DoubleDataAdapterXml::initialize("zAngle", "Z Angle", "", 0, ssc::DoubleRange(-M_PI,M_PI,M_PI/180),0);
//  connect(mZAngle.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
//  mZAngle->setInternal2Display(180/M_PI);

//  toptopLayout->addWidget(new QLabel("tekst pre"));
//  toptopLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mXAngle));
//  toptopLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mYAngle));
//  toptopLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mZAngle));
//  toptopLayout->addWidget(new QLabel("tekst post"));

//  QSize mMinBarSize = QSize(20,25);
//
//  MousePadWidget* rotateYWidget = new MousePadWidget(this, mMinBarSize);
//  rotateYWidget->setFixedYPos(true);
//  rotateYWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//  rotateYWidget->setLineWidth(3);
//  connect(rotateYWidget, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateYSlot(QPointF)));
//  toptopLayout->addWidget(rotateYWidget);

  this->setMatrix(ssc::Transform3D());

  toptopLayout->addStretch();
}

void AffineMatrixWidget::addAngleControls(QString uid, QString name, int index, QVBoxLayout* layout)
{
  QHBoxLayout* hLayout = new QHBoxLayout;

  ssc::DoubleDataAdapterXmlPtr adapter = ssc::DoubleDataAdapterXml::initialize(uid, name, "", 0, ssc::DoubleRange(-M_PI,M_PI,M_PI/180),0);
  connect(adapter.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
  adapter->setInternal2Display(180/M_PI);
  hLayout->addWidget(new ssc::SpinBoxGroupWidget(this, adapter));

//  QFrame* frame = new QFrame(this);
//  frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
//  frame->setLineWidth(3);
//  QVBoxLayout* fLayout = new QVBoxLayout;
//  fLayout->setMargin(0);
//  frame->setLayout(fLayout);

  QSize mMinBarSize = QSize(20,20);
  MousePadWidget* pad = new MousePadWidget(this, mMinBarSize);
  pad->setFixedYPos(true);
//  pad->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//  pad->setLineWidth(3);
  hLayout->addWidget(pad);

  // use QtSignalAdapters library to work magic:
  QtSignalAdapters::connect1<void(QPointF)>(pad, SIGNAL(mouseMoved(QPointF)),
      boost::bind(&AffineMatrixWidget::rotateSlot, this, _1, index));

//  connect(pad, SIGNAL(mouseMoved(QPointF)), this, SLOT(rotateYSlot(QPointF)));
//  hLayout->addWidget(frame);

  layout->addLayout(hLayout);
  mAngleAdapter[index] = adapter;
}

void AffineMatrixWidget::rotateSlot(QPointF delta, int index)
{
//  std::cout << "AffineMatrixWidget::rotateSlot " << index << " " << delta.x() << std::endl;

  double scale = M_PI;
  double factor = scale * delta.x();
  double current = mAngleAdapter[index]->getValue();
  mAngleAdapter[index]->setValue(current + factor);
}

/*
connect0<void()>(pushButton, SIGNAL(clicked()),
   boost::bind(&IceProxy::Motors::MotorControl::move, motorControlPrx_.get());

connect1<void(int)>(spinBox, SIGNAL(valueChanged(int)),
    boost::bind(&IceProxy::Motors::MotorControl::setMotorMoveCounts, motorControlPrx_.get(), _1));*/

AffineMatrixWidget::~AffineMatrixWidget()
{
}

void AffineMatrixWidget::setMatrix(const ssc::Transform3D& M)
{
  mFrame = ssc::Frame3D::create(M);
  this->updateValues();
}

ssc::Transform3D AffineMatrixWidget::getMatrix() const
{
  return mFrame.transform();
}

// http://en.wikipedia.org/wiki/Rotation_matrix
// http://en.wikipedia.org/wiki/Rotation_representation_(mathematics)#Conversion_formulae_between_representations

void AffineMatrixWidget::changedSlot()
{
  if (recursive)
    return;
  recursive = true;
  ssc::Vector3D xyz(mAngleAdapter[0]->getValue(),mAngleAdapter[1]->getValue(),mAngleAdapter[2]->getValue());
  mFrame.setEulerXYZ(xyz);

  emit changed();
//  this->updateValues();
  recursive = false;
}

void AffineMatrixWidget::updateValues()
{
  QString t = qstring_cast(this->getMatrix());
//  std::cout << "mTextEdit " << mTextEdit << " " << t << std::endl;
  mTextEdit->setText(t);

  ssc::Vector3D xyz = mFrame.getEulerXYZ();

  mAngleAdapter[0]->setValue(xyz[0]);
  mAngleAdapter[1]->setValue(xyz[1]);
  mAngleAdapter[2]->setValue(xyz[2]);
}

}
