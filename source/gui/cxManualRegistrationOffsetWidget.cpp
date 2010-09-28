#include "cxManualRegistrationOffsetWidget.h"

#include <QLabel>
#include <QGridLayout>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "cxToolManager.h"
#include "cxRegistrationManager.h"

namespace cx
{
ManualRegistrationOffsetWidget::ManualRegistrationOffsetWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mVerticalLayout(new QVBoxLayout(this)),
    mOffsetLabel(new QLabel(QString("Manual offset:"), this)),
    mOffsetsGridLayout(new QGridLayout()),
    mXLabel(new QLabel(QString("X "), this)),
    mYLabel(new QLabel(QString("Y "), this)),
    mZLabel(new QLabel(QString("Z "), this)),
    mXOffsetSlider(new QSlider(Qt::Horizontal, this)),
    mYOffsetSlider(new QSlider(Qt::Horizontal, this)),
    mZOffsetSlider(new QSlider(Qt::Horizontal, this)),
    mXSpinBox(new QSpinBox(this)),
    mYSpinBox(new QSpinBox(this)),
    mZSpinBox(new QSpinBox(this)),
    mResetOffsetButton(new QPushButton("Clear offset", this)),
    mMinValue(-200),
    mMaxValue(200),
    mDefaultValue(0)
{
  this->setObjectName("ManualRegistrationOffsetWidget");
  this->setWindowTitle("Manual Registration Offset");

  mResetOffsetButton->setDisabled(true);
  connect(mResetOffsetButton, SIGNAL(clicked()), this, SLOT(resetOffsetSlot()));

  //registrationmanager
  connect(registrationManager(), SIGNAL(patientRegistrationPerformed()), this, SLOT(activateManualRegistrationFieldSlot()));
  connect(ssc::toolManager(), SIGNAL(rMprChanged()), this, SLOT(activateManualRegistrationFieldSlot()));

  //sliders
  mXOffsetSlider->setRange(mMinValue,mMaxValue);
  mXOffsetSlider->setValue(mDefaultValue);
  mYOffsetSlider->setRange(mMinValue,mMaxValue);
  mYOffsetSlider->setValue(mDefaultValue);
  mZOffsetSlider->setRange(mMinValue,mMaxValue);
  mZOffsetSlider->setValue(mDefaultValue);

  //spinboxes
  mXSpinBox->setRange(mMinValue,mMaxValue);
  mXSpinBox->setValue(mDefaultValue);
  mYSpinBox->setRange(mMinValue,mMaxValue);
  mYSpinBox->setValue(mDefaultValue);
  mZSpinBox->setRange(mMinValue,mMaxValue);
  mZSpinBox->setValue(mDefaultValue);

  //connect sliders to the spinbox
  connect(mXOffsetSlider, SIGNAL(valueChanged(int)),
          mXSpinBox, SLOT(setValue(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          mYSpinBox, SLOT(setValue(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          mZSpinBox, SLOT(setValue(int)));
  connect(mXSpinBox, SIGNAL(valueChanged(int)),
          mXOffsetSlider, SLOT(setValue(int)));
  connect(mYSpinBox, SIGNAL(valueChanged(int)),
          mYOffsetSlider, SLOT(setValue(int)));
  connect(mZSpinBox, SIGNAL(valueChanged(int)),
          mZOffsetSlider, SLOT(setValue(int)));

  //connect sliders registrationmanager,
  //spinboxes are connected to the sliders so no need to connect them to the
  //registration manager as well
  connect(mXOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));
  connect(mYOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));
  connect(mZOffsetSlider, SIGNAL(valueChanged(int)),
          this, SLOT(setOffsetSlot(int)));

  //layout
  mOffsetsGridLayout->addWidget(mXLabel, 0, 0);
  mOffsetsGridLayout->addWidget(mYLabel, 1, 0);
  mOffsetsGridLayout->addWidget(mZLabel, 2, 0);
  mOffsetsGridLayout->addWidget(mXOffsetSlider, 0, 1);
  mOffsetsGridLayout->addWidget(mYOffsetSlider, 1, 1);
  mOffsetsGridLayout->addWidget(mZOffsetSlider, 2, 1);
  mOffsetsGridLayout->addWidget(mXSpinBox, 0, 2);
  mOffsetsGridLayout->addWidget(mYSpinBox, 1, 2);
  mOffsetsGridLayout->addWidget(mZSpinBox, 2, 2);

  mVerticalLayout->addWidget(mOffsetLabel);
  mVerticalLayout->addLayout(mOffsetsGridLayout);
  mVerticalLayout->addWidget(mResetOffsetButton);

  this->activateManualRegistrationFieldSlot();
}

ManualRegistrationOffsetWidget::~ManualRegistrationOffsetWidget()
{}

void ManualRegistrationOffsetWidget::resetOffsetSlot()
{
  if(mXOffsetSlider->value() != mDefaultValue ||
     mYOffsetSlider->value() != mDefaultValue ||
     mZOffsetSlider->value() != mDefaultValue)
  {
    mXOffsetSlider->setValue(mDefaultValue);
    mYOffsetSlider->setValue(mDefaultValue);
    mZOffsetSlider->setValue(mDefaultValue);
    mResetOffsetButton->setDisabled(true);
    this->setOffsetSlot(mDefaultValue);
  }
}

QString ManualRegistrationOffsetWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Manual offset.</h3>"
      "<p>Method used for correcting small mismatches.</p>"
      "<p><i>Drag the sliders to adjust the data set.</i></p>"
      "</html>";
}

void ManualRegistrationOffsetWidget::setOffsetSlot(int value)
{
  mResetOffsetButton->setEnabled(true);

  ssc::Transform3D offset;
  offset[0][3] = mXOffsetSlider->value();
  offset[1][3] = mYOffsetSlider->value();
  offset[2][3] = mZOffsetSlider->value();
  registrationManager()->setManualPatientRegistrationOffsetSlot(offset);
}

void ManualRegistrationOffsetWidget::activateManualRegistrationFieldSlot()
{
  this->setEnabled(!ssc::similar(*ssc::toolManager()->get_rMpr(), ssc::Transform3D()));
}


} //namespace cx
