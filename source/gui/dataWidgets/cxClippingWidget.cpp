/*
 * cxClippingWidget.cpp
 *
 *  \date Aug 25, 2010
 *      \author christiana
 */
#include "cxClippingWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "sscStringDataAdapter.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDefinitionStrings.h"
#include "cxInteractiveClipper.h"
#include "cxViewManager.h"
#include "sscDataManager.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx
{

ClipPlaneStringDataAdapter::ClipPlaneStringDataAdapter(InteractiveClipperPtr clipper) :
	mInteractiveClipper(clipper)
{
	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QString ClipPlaneStringDataAdapter::getValueName() const
{
	return "Slice Plane";
}
bool ClipPlaneStringDataAdapter::setValue(const QString& value)
{
	ssc::PLANE_TYPE plane = string2enum<ssc::PLANE_TYPE> (value);
	if (plane == mInteractiveClipper->getSlicePlane())
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
	return "Chose the slice plane to clip with.";
}
QStringList ClipPlaneStringDataAdapter::getValueRange() const
{
	std::vector<ssc::PLANE_TYPE> planes = mInteractiveClipper->getAvailableSlicePlanes();
	QStringList retval;
	//retval << ""; // removed this. No idea why we need an empty entry.
	for (unsigned i = 0; i < planes.size(); ++i)
		retval << qstring_cast(planes[i]);
	return retval;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ClippingWidget::ClippingWidget(QWidget* parent) :
	BaseWidget(parent, "ClippingWidget", "Clip")
{
	mInteractiveClipper = viewManager()->getClipper();
	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SLOT(clipperChangedSlot()));

	mImageAdapter = SelectImageStringDataAdapter::New();
	ssc::LabeledComboBoxWidget* imageCombo = new ssc::LabeledComboBoxWidget(this, mImageAdapter);
	connect(mImageAdapter.get(), SIGNAL(changed()), this, SLOT(imageChangedSlot()));
//	mImageAdapter->setValue();

	this->setToolTip(this->defaultWhatsThis());

	QVBoxLayout* layout = new QVBoxLayout(this);

	QGroupBox* activeClipGroupBox = new QGroupBox("Interactive clipper");
	activeClipGroupBox->setToolTip(this->defaultWhatsThis());
	layout->addWidget(activeClipGroupBox);
	QVBoxLayout* activeClipLayout = new QVBoxLayout(activeClipGroupBox);

	mPlaneAdapter = ClipPlaneStringDataAdapter::New(mInteractiveClipper);
	ssc::LabeledComboBoxWidget* combo = new ssc::LabeledComboBoxWidget(this, mPlaneAdapter);

	mUseClipperCheckBox = new QCheckBox("Use Clipper");
	mUseClipperCheckBox->setToolTip("Turn on interactive clipping for the selected volume.");
	connect(mUseClipperCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(useClipper(bool)));
	activeClipLayout->addWidget(mUseClipperCheckBox);
	activeClipLayout->addWidget(imageCombo);
	activeClipLayout->addWidget(combo);
	mInvertPlaneCheckBox = new QCheckBox("Invert plane");
	mInvertPlaneCheckBox->setToolTip("Use the inverse (mirror) of the selected slice plane.");
	connect(mInvertPlaneCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(invertPlane(bool)));
	activeClipLayout->addWidget(mInvertPlaneCheckBox);

	QPushButton* saveButton = new QPushButton("Save clip plane");
	saveButton->setToolTip("Save the interactive plane as a clip plane in the selected volume.");
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClickedSlot()));
	//saveButton->setEnabled(false);
	QPushButton* clearButton = new QPushButton("Clear saved planes");
	clearButton->setToolTip("Remove all saved clip planes from the selected volume");
	connect(clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClickedSlot()));
	//clearButton->setEnabled(false);
	activeClipLayout->addWidget(saveButton);
	layout->addWidget(clearButton);

	layout->addStretch();

	this->clipperChangedSlot();
}

QString ClippingWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Functonality for clipping a volume</h3>"
		"<p>"
		"Define clip planes in a volume. The interactive clipper is attached "
		"to the active tool, and clips the active volume according to a slice "
		"definition. "
		"</p>"
		"<p>"
		"The current clip can also be saved along with the volume. This can be "
		"done several times."
		"</p>"
		"<p><i></i></p>"
		"</html>";
}

void ClippingWidget::clipperChangedSlot()
{
	mUseClipperCheckBox->setChecked(mInteractiveClipper->getUseClipper());
	mInvertPlaneCheckBox->setChecked(mInteractiveClipper->getInvertPlane());
}

void ClippingWidget::imageChangedSlot()
{
	mInteractiveClipper->setImage(ssc::dataManager()->getImage(mImageAdapter->getValue()));
}

void ClippingWidget::clearButtonClickedSlot()
{
	mInteractiveClipper->clearClipPlanesInVolume();
}

void ClippingWidget::saveButtonClickedSlot()
{
	mInteractiveClipper->saveClipPlaneToVolume();
}

}
