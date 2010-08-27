/*
 * cxClippingWidget.cpp
 *
 *  Created on: Aug 25, 2010
 *      Author: christiana
 */
#include "cxClippingWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "sscStringDataAdapter.h"
#include "sscStringWidgets.h"
#include "sscDefinitionStrings.h"

namespace cx
{

ClipPlaneStringDataAdapter::ClipPlaneStringDataAdapter(InteractiveClipperPtr clipper) : mInteractiveClipper(clipper)
{
  connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QString ClipPlaneStringDataAdapter::getValueName() const
{
  return "Slice Plane";
}
bool ClipPlaneStringDataAdapter::setValue(const QString& value)
{
  ssc::PLANE_TYPE plane = string2enum<ssc::PLANE_TYPE>(string_cast(value));
  if (plane==mInteractiveClipper->getSlicePlane())
    return false;
  mInteractiveClipper->setSlicePlane(plane);
  return true;
}
QString ClipPlaneStringDataAdapter::getValue() const
{
  return qstring_cast(mInteractiveClipper->getSlicePlane());
}
QString ClipPlaneStringDataAdapter::getHelp() const
{
  return "chose the slice plane to clip with";
}
QStringList ClipPlaneStringDataAdapter::getValueRange() const
{
  std::vector<ssc::PLANE_TYPE> planes = mInteractiveClipper->getAvailableSlicePlanes();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<planes.size(); ++i)
    retval << qstring_cast(planes[i]);
  return retval;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ClippingWidget::ClippingWidget(QWidget* parent) : QWidget(parent)
{
  mInteractiveClipper = viewManager()->getClipper();
  connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SLOT(clipperChangedSlot()));

  this->setObjectName("ClippingWidget");
  this->setWindowTitle("Clip");

  QVBoxLayout* layout = new QVBoxLayout(this);

  QGroupBox* activeClipGroupBox = new QGroupBox("Interactive clipper");
  layout->addWidget(activeClipGroupBox);
  QVBoxLayout* activeClipLayout = new QVBoxLayout(activeClipGroupBox);

  mPlaneAdapter = ClipPlaneStringDataAdapter::New(mInteractiveClipper);
  ssc::ComboGroupWidget* combo = new ssc::ComboGroupWidget(this, mPlaneAdapter);

  mUseClipperCheckBox = new QCheckBox("Use Clipper");
  connect(mUseClipperCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(useClipper(bool)));
  activeClipLayout->addWidget(mUseClipperCheckBox);
  activeClipLayout->addWidget(combo);
  mInvertPlaneCheckBox = new QCheckBox("Invert plane");
  connect(mInvertPlaneCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(invertPlane(bool)));
  activeClipLayout->addWidget(mInvertPlaneCheckBox);

  QPushButton* saveButton = new QPushButton("Save clip plane");
  saveButton->setEnabled(false);
  QPushButton* clearButton = new QPushButton("Clear saved planes");
  clearButton->setEnabled(false);
  activeClipLayout->addWidget(saveButton);
  layout->addWidget(clearButton);

  layout->addStretch();

  this->clipperChangedSlot();
}

void ClippingWidget::clipperChangedSlot()
{
  mUseClipperCheckBox->setChecked(mInteractiveClipper->getUseClipper());
  mInvertPlaneCheckBox->setChecked(mInteractiveClipper->getInvertPlane());
}

}
