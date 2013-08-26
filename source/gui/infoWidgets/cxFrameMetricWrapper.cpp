#include "cxFrameMetricWrapper.h"

#include <QHBoxLayout>
#include "sscLabeledComboBoxWidget.h"

namespace cx {

FrameMetricWrapper::FrameMetricWrapper(cx::FrameMetricPtr data) : mData(data)
{
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

QWidget* FrameMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	QString value;// = qstring_cast(mData->getFrame());
	std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces(true);
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
		range << spaces[i].toString();

	mSpaceSelector = ssc::StringDataAdapterXml::initialize("selectSpace",
			"Space",
			"Select coordinate system to store position in.",
			value,
			range,
			QDomNode());
	hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mSpaceSelector));

	mFrameWidget = new Transform3DWidget(widget);
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(mFrameWidget, SIGNAL(changed()), this, SLOT(frameWidgetChangedSlot()));
	topLayout->addWidget(mFrameWidget);

	QPushButton* sampleButton = new QPushButton("Sample");
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));
	this->dataChangedSlot();

	return widget;
}

QString FrameMetricWrapper::getValue() const
{
	/*  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mData->getSpace(), ssc::CoordinateSystem(ssc::csREF));
//  ssc::Vector3D p0_r = rM0.coord(mData->getCoordinate());
//  return "pt_r="+qstring_cast(p0_r);
  int w=3;
  return QString("%1 %2 %3").arg(p0_r[0], w, 'f', 1).arg(p0_r[1], w, 'f', 1).arg(p0_r[2], w, 'f', 1);
	 */
	//  return mData->matrixAsSingleLineString();
	return mData->pointAsSingleLineString();
}

ssc::DataPtr FrameMetricWrapper::getData() const
{
	return mData;
}

QString FrameMetricWrapper::getType() const
{
	return "frame";
}

QString FrameMetricWrapper::getArguments() const
{
	/*  ssc::Vector3D p = mData->getCoordinate();
  int w=1;
  QString coord = QString("(%1 %2 %3)").arg(p[0], w, 'f', 1).arg(p[1], w, 'f', 1).arg(p[2], w, 'f', 1);
  if (mData->getSpace().mId==ssc::csREF)
  	coord = ""; // ignore display of coord if in ref space

  return mData->getSpace().toString() + " " + coord;*/
	return mData->getSpace().toString();
}


void FrameMetricWrapper::moveToToolPosition()
{
	ssc::CoordinateSystem ref = ssc::SpaceHelpers::getR();
	ssc::Transform3D qMt = ssc::SpaceHelpers::getDominantToolTipTransform(mData->getSpace());
	std::cout << "set frame " << qMt << std::endl;
	mData->setFrame(qMt);
}

void FrameMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
	if (space.mId==ssc::csCOUNT)
		return;
	mData->setSpace(space);
}

void FrameMetricWrapper::dataChangedSlot()
{
	mInternalUpdate = true;
	mSpaceSelector->setValue(mData->getSpace().toString());
	mFrameWidget->setMatrix(mData->getFrame());
	mInternalUpdate = false;
}

void FrameMetricWrapper::frameWidgetChangedSlot()
{
	if (mInternalUpdate)
		return;
	ssc::Transform3D matrix = mFrameWidget->getMatrix();
	mData->setFrame(matrix);
}


} //namespace cx
