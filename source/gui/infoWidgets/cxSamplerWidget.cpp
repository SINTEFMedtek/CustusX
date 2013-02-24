// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxSamplerWidget.h"

#include "sscCoordinateSystemHelpers.h"
#include <vtkImageData.h>
#include "cxToolManager.h"
//#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "cxSettings.h"

namespace cx
{

SamplerWidget::SamplerWidget(QWidget* parent) :
  BaseWidget(parent, "SamplerWidget", "Point Sampler")
{
	mListener.reset(new ssc::CoordinateSystemListener(ssc::Space(ssc::csREF)));
	connect(mListener.get(), SIGNAL(changed()), this, SLOT(setModified()));

	mActiveTool = DominantToolProxy::New();
	connect(mActiveTool.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(setModified()));
	connect(mActiveTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), SLOT(setModified()));
//	connect(mActiveTool.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(setModified2()));
//	connect(mActiveTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), SLOT(setModified2()));
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(spacesChangedSlot()));
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(spacesChangedSlot()));

	mLayout = new QHBoxLayout(this);
	mLayout->setMargin(4);
	mLayout->setSpacing(4);

//	QString value;// = qstring_cast(mData->getFrame());
//    std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces(true);
//    QStringList range;
//    for (unsigned i=0; i<spaces.size(); ++i)
//      range << spaces[i].toString();

	mAdvancedAction = this->createAction(this,
	                                     QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
	                                     "Details",
	                                     "Show Advanced options",
	                                     SLOT(toggleAdvancedSlot()),
	                                     mLayout,
	                                     new CXSmallToolButton());

	mAdvancedWidget = new QWidget(this);
	mAdvancedLayout = new QHBoxLayout(mAdvancedWidget);
	mAdvancedLayout->setMargin(0);
	mLayout->addWidget(mAdvancedWidget);

    mSpaceSelector = ssc::StringDataAdapterXml::initialize("selectSpace",
        "Space",
        "Select coordinate system to store position in.",
	    "",
		QStringList(),
        QDomNode());
	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spacesChangedSlot()));
	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(setModified()));
	QString space = settings()->value("sampler/Space", ssc::Space(ssc::csREF).toString()).toString();
	mSpaceSelector->setValue(space);
	ssc::LabeledComboBoxWidget* spaceSelectorWidget = new ssc::LabeledComboBoxWidget(this, mSpaceSelector);
	spaceSelectorWidget->showLabel(false);
    mAdvancedLayout->addWidget(spaceSelectorWidget);
	this->spacesChangedSlot();

	mCoordLineEdit = new QLineEdit(this);
	mCoordLineEdit->setStyleSheet("QLineEdit { width : 30ex; }"); // enough for "-xxx.x, -xxx.x, -xxx.x - xxxx" plus some slack
	mCoordLineEdit->setSizePolicy(QSizePolicy::Fixed,
                                  mCoordLineEdit->sizePolicy().verticalPolicy());
	mCoordLineEdit->setReadOnly(true);
    mLayout->addWidget(mCoordLineEdit);

	this->showAdvanced();
	this->setModified();
}

SamplerWidget::~SamplerWidget()
{}

QString SamplerWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Utility for sampling the current tool point</h3>"
	  "<p>Displays the current tool tip position in a selected coordinate system.</p>"
	  "<p><i></i></p>"
	  "</html>";
}

void SamplerWidget::toggleAdvancedSlot()
{
	settings()->setValue("sampler/ShowDetails", !settings()->value("sampler/ShowDetails", "true").toBool());
	mAdvancedWidget->setVisible(!mAdvancedWidget->isVisible());
	this->showAdvanced();
}

void SamplerWidget::showAdvanced()
{
	bool on = settings()->value("sampler/ShowDetails").toBool();
	mAdvancedWidget->setVisible(on);
}

void SamplerWidget::setModified2()
{
//	std::cout << "SamplerWidget::setModified2()" << std::endl;
}

void SamplerWidget::spacesChangedSlot()
{
	ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
	settings()->setValue("sampler/Space", space.toString());

//	QString value;// = qstring_cast(mData->getFrame());
	std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces(true);
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
	  range << spaces[i].toString();

	mSpaceSelector->setValueRange(range);
	mSpaceSelector->setValue(space.toString());
	mListener->setSpace(space);
}

void SamplerWidget::prePaintEvent()
{
//	std::cout << "SamplerWidget::prePaintEvent()" << std::endl;
	ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
	ssc::Vector3D p = ssc::SpaceHelpers::getDominantToolTipPoint(space, true);
	int w=1;
//	mCoordLineEdit->setText(qstring_cast(p));
	QString coord = QString("%1, %2, %3").arg(p[0], w, 'f', 1).arg(p[1], w, 'f', 1).arg(p[2], w, 'f', 1);

	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	if (image)
	{
		ssc::Vector3D p = ssc::SpaceHelpers::getDominantToolTipPoint(ssc::Space(ssc::csDATA_VOXEL,"active"), true);
//		void* ptr = image->getBaseVtkImageData()->GetScalarPointer(p.begin());
		ssc::IntBoundingBox3D bb(Eigen::Vector3i(0,0,0),
		                         Eigen::Vector3i(image->getBaseVtkImageData()->GetDimensions())-Eigen::Vector3i(1,1,1));
		if (bb.contains(p.cast<int>()))
		{
			double val = image->getBaseVtkImageData()->GetScalarComponentAsFloat(p[0], p[1], p[2], 0);
			int intVal = val;
			coord += QString(" I=%1").arg(intVal);
		}
	}

	mCoordLineEdit->setText(coord);
	mCoordLineEdit->setStatusTip(QString("Position of active tool tip in %1 space\n"
	                                     "and the intensity of the active volume in that position").arg(space.toString()));
	mCoordLineEdit->setToolTip(mCoordLineEdit->statusTip());
}


} // namespace cx

