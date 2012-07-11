#include <cxTransform3DWidget.h>

#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QFontMetrics>

#include "sscTypeConversions.h"

#include "sscDoubleDataAdapterXml.h"
#include "sscDoubleWidgets.h"
#include <cmath>
#include "cxTrackPadWidget.h"

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
    ssc::Transform3D M = ssc::createTransformRotateX(M_PI_4) *
        ssc::createTransformRotateZ(M_PI_4) *
        ssc::createTransformTranslate(ssc::Vector3D(1,2,M_PI));

    QString text = qstring_cast(M).split("\n")[0];
    QRect rect = QFontMetrics(this->font()).boundingRect(text);
    QSize s(rect.width()*1.1+5, 4*rect.height()*1.1+5);
    return s;
  }
};

template<class T>
QAction* Transform3DWidget::createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot)
{
  QAction* action = new QAction(QIcon(iconName), text, this);
  action->setStatusTip(tip);
  action->setToolTip(tip);
  connect(action, SIGNAL(triggered()), this, slot);
  QToolButton* button = new QToolButton();
  button->setDefaultAction(action);
  layout->addWidget(button);
  return action;
}


Transform3DWidget::Transform3DWidget(QWidget* parent) :
    BaseWidget(parent, "Transform3DWidget", "Transform 3D")
{
  recursive = false;
  mBlockChanges = false;
  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  toptopLayout->setMargin(4);
  QHBoxLayout* mLayout = new QHBoxLayout;
  mLayout->setMargin(0);
  toptopLayout->addLayout(mLayout);

  mTextEdit = new MatrixTextEdit;
  mTextEdit->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);
  mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
  mTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textEditChangedSlot()));

  mLayout->addWidget(mTextEdit, 1);

  mEditAction = this->createAction(mLayout,
      ":/icons/open_icon_library/png/64x64/actions/system-run-5.png",
      "Edit",
      "Toggle Edit Matrix",
      SLOT(toggleEditSlot()));

//  mLayout->addStretch();

  aGroupBox = new QFrame(this);
  QVBoxLayout* aLayout = new QVBoxLayout;
  aGroupBox->setLayout(aLayout);
  aGroupBox->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  aGroupBox->setLineWidth(3);
  aLayout->setMargin(4);
  toptopLayout->addWidget(aGroupBox);

  this->addAngleControls("xAngle", "X Angle", 0, aLayout);
  this->addAngleControls("yAngle", "Y Angle", 1, aLayout);
  this->addAngleControls("zAngle", "Z Angle", 2, aLayout);

  tGroupBox = new QFrame(this);
  QVBoxLayout* tLayout = new QVBoxLayout;
  tGroupBox->setLayout(tLayout);
  tGroupBox->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  tGroupBox->setLineWidth(3);
  tLayout->setMargin(4);
  toptopLayout->addWidget(tGroupBox);

  this->addTranslationControls("xTranslation", "X", 0, tLayout);
  this->addTranslationControls("yTranslation", "Y", 1, tLayout);
  this->addTranslationControls("zTranslation", "Z", 2, tLayout);

  this->setMatrix(ssc::Transform3D::Identity());

  toptopLayout->addStretch();

  this->setEditable(false);
}

QString Transform3DWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Transform 3D</h3>"
    "<p>Lets you display and manipulat an affine matrix, i.e. a rotation+translation matrix.</p>"
    "<p><i></i></p>"
    "</html>";
}

void Transform3DWidget::textEditChangedSlot()
{
  bool ok = false;
  ssc::Transform3D M = ssc::Transform3D::fromString(mTextEdit->toPlainText(), &ok);
  // ignore setting if invalid matrix or no real change done (hopefully, this allows trivial editing without text reset)
  if (!ok)
    return;
  if (ssc::similar(M, this->getMatrix()))
    return;

  this->setMatrix(M);
}

void Transform3DWidget::toggleEditSlot()
{
  bool visible = tGroupBox->isVisible();
  this->setEditable(!visible);
}

void Transform3DWidget::setEditable(bool edit)
{
  mTextEdit->setReadOnly(!edit);
  aGroupBox->setVisible(edit);
  tGroupBox->setVisible(edit);
}

void Transform3DWidget::addAngleControls(QString uid, QString name, int index, QVBoxLayout* layout)
{
  QHBoxLayout* hLayout = new QHBoxLayout;

  ssc::DoubleDataAdapterXmlPtr adapter = ssc::DoubleDataAdapterXml::initialize(uid, name, "", 0, ssc::DoubleRange(-M_PI,M_PI,M_PI/180),1);
  connect(adapter.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
  adapter->setInternal2Display(180/M_PI);
  hLayout->addWidget(new ssc::SpinBoxGroupWidget(this, adapter));

  QSize mMinBarSize = QSize(20,20);
  ssc::MousePadWidget* pad = new ssc::MousePadWidget(this, mMinBarSize);
  pad->setFixedYPos(true);
  hLayout->addWidget(pad);

  // use QtSignalAdapters library to work magic:
  QtSignalAdapters::connect1<void(QPointF)>(pad, SIGNAL(mouseMoved(QPointF)),
      boost::bind(&Transform3DWidget::rotateSlot, this, _1, index));

  layout->addLayout(hLayout);
  mAngleAdapter[index] = adapter;
}

void Transform3DWidget::addTranslationControls(QString uid, QString name, int index, QVBoxLayout* layout)
{
  QHBoxLayout* hLayout = new QHBoxLayout;

  ssc::DoubleDataAdapterXmlPtr adapter = ssc::DoubleDataAdapterXml::initialize(uid, name, "", 0, ssc::DoubleRange(-10000,10000,0.1),1);
  connect(adapter.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
  adapter->setInternal2Display(1.0);
  hLayout->addWidget(new ssc::SpinBoxGroupWidget(this, adapter));

  QSize mMinBarSize = QSize(20,20);
  ssc::MousePadWidget* pad = new ssc::MousePadWidget(this, mMinBarSize);
  pad->setFixedYPos(true);
  hLayout->addWidget(pad);

  // use QtSignalAdapters library to work magic:
  QtSignalAdapters::connect1<void(QPointF)>(pad, SIGNAL(mouseMoved(QPointF)),
      boost::bind(&Transform3DWidget::translateSlot, this, _1, index));

  layout->addLayout(hLayout);
  mTranslationAdapter[index] = adapter;
}

void Transform3DWidget::rotateSlot(QPointF delta, int index)
{
  double scale = M_PI_2;
  double factor = scale * delta.x();
  double current = mAngleAdapter[index]->getValue();
  mAngleAdapter[index]->setValue(current + factor);
}

void Transform3DWidget::translateSlot(QPointF delta, int index)
{
  double scale = 20;
  double factor = scale * delta.x();
  double current = mTranslationAdapter[index]->getValue();
  mTranslationAdapter[index]->setValue(current + factor);
}


Transform3DWidget::~Transform3DWidget()
{
}

void Transform3DWidget::setMatrix(const ssc::Transform3D& M)
{
  mDecomposition.reset(M);
  this->updateValues();
  emit changed();
}

ssc::Transform3D Transform3DWidget::getMatrix() const
{
  return mDecomposition.getMatrix();
}

// http://en.wikipedia.org/wiki/Rotation_matrix
// http://en.wikipedia.org/wiki/Rotation_representation_(mathematics)#Conversion_formulae_between_representations

void Transform3DWidget::changedSlot()
{
  if (recursive || mBlockChanges)
    return;
  recursive = true;
  ssc::Vector3D xyz(mAngleAdapter[0]->getValue(),mAngleAdapter[1]->getValue(),mAngleAdapter[2]->getValue());
  mDecomposition.setAngles(xyz);

  ssc::Vector3D t(mTranslationAdapter[0]->getValue(),mTranslationAdapter[1]->getValue(),mTranslationAdapter[2]->getValue());
  mDecomposition.setPosition(t);

  this->updateValues();
  emit changed();
  recursive = false;
}

namespace
{
	/**Wrap the angle into the interval -M_PI..M_PI
	 *
	 */
	double wrapAngle(double angle)
	{
		angle = fmod(angle, M_PI * 2);
		if (angle > M_PI)
			angle -= M_PI * 2;
		if (angle < -M_PI)
			angle += M_PI * 2;
		return angle;
	}
}

void Transform3DWidget::updateValues()
{
  QString M = qstring_cast(this->getMatrix());

  mTextEdit->blockSignals(true);
  int textPos = mTextEdit->textCursor().position();
  mTextEdit->setText(M);
  QTextCursor cursor = mTextEdit->textCursor();
  cursor.setPosition(textPos);
  mTextEdit->setTextCursor(cursor);
  mTextEdit->blockSignals(false);

  ssc::Vector3D xyz = mDecomposition.getAngles();

  mBlockChanges = true;

  mAngleAdapter[0]->setValue(wrapAngle(xyz[0]));
  mAngleAdapter[1]->setValue(wrapAngle(xyz[1]));
  mAngleAdapter[2]->setValue(wrapAngle(xyz[2]));

  ssc::Vector3D t = mDecomposition.getPosition();
  mTranslationAdapter[0]->setValue(t[0]);
  mTranslationAdapter[1]->setValue(t[1]);
  mTranslationAdapter[2]->setValue(t[2]);

  mBlockChanges = false;
}
}
