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

#include "cxWirePhantomWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include "cxBinaryThresholdImageFilterWidget.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "cxColorSelectButton.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "cxRegistrationManager.h"
#include "cxDataLocations.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxViewManager.h"
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "boost/bind.hpp"

namespace cx
{
//------------------------------------------------------------------------------
WirePhantomWidget::WirePhantomWidget(RegistrationManagerPtr regManager, QWidget* parent) :
		RegistrationBaseWidget(regManager, parent, "WirePhantomWidget", "Wire Phantom")
{
	connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this,
					SLOT(activeImageChangedSlot(const QString&)));

	mLayout = new QVBoxLayout(this);
	mSegmentationWidget = new BinaryThresholdImageFilterWidget(this);
	mCenterlineWidget = new CenterlineWidget(this);

	mUSImageInput = SelectImageStringDataAdapter::New();
	mUSImageInput->setValueName("US Image: ");
	mUSImageInput->setHelp("Select an US volume acquired from the wire phantom.");
	connect(mUSImageInput.get(), SIGNAL(imageChanged(QString)), mSegmentationWidget, SLOT(setImageInputSlot(QString)));

	mSegmentationOutput = SelectImageStringDataAdapter::New();
	mSegmentationOutput->setValueName("Output: ");
	connect(mSegmentationOutput.get(), SIGNAL(imageChanged(QString)), mCenterlineWidget,
			SLOT(setImageInputSlot(QString)));

	mCenterlineOutput = SelectDataStringDataAdapter::New();
	mCenterlineOutput->setValueName("Output: ");
	connect(mCenterlineOutput.get(), SIGNAL(dataChanged(QString)), this, SLOT(setImageSlot(QString)));

	QPushButton* showCrossButton = new QPushButton("Show Cross");
	showCrossButton->setToolTip("Load the centerline description of the wire cross");
	connect(showCrossButton, SIGNAL(clicked()), this, SLOT(loadNominalCross()));

	mCenterlineWidget->setDefaultColor(QColor("red"));
	mSegmentationWidget->setDefaultColor(QColor("red"));

//	this->setColorSlot(QColor("green"));
//
//	ColorSelectButton* colorButton = new ColorSelectButton("Color");
//	colorButton->setColor(QColor("green"));
//	colorButton->setToolTip("Select color to use when generating surfaces and centerlines.");
//	connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setColorSlot(QColor)));

	mMeasureButton = new QPushButton("Execute");
	mMeasureButton->setToolTip("Measure deviation of input volume from nominal wire cross.");
	connect(mMeasureButton, SIGNAL(clicked()), this, SLOT(measureSlot()));

	QLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mMeasureButton);

	mResults = new QTextEdit;

	mLayout->addWidget(showCrossButton);
	mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mUSImageInput));
	mLayout->addWidget(this->createHorizontalLine());
	mLayout->addWidget(
			this->createMethodWidget(mSegmentationWidget, new ssc::LabeledComboBoxWidget(this, mSegmentationOutput),
					"Segmentation"));
	mLayout->addWidget(this->createHorizontalLine());
	mLayout->addWidget(
			this->createMethodWidget(mCenterlineWidget, new ssc::LabeledComboBoxWidget(this, mCenterlineOutput),
					"Centerline"));
	mLayout->addWidget(this->createHorizontalLine());
	mLayout->addLayout(buttonsLayout);
	mLayout->addWidget(mResults, 1);
	mLayout->addStretch();

//  connect(mResampleWidget, SIGNAL(outputImageChanged(QString)), this , SLOT(resampleOutputArrived(QString)));
	connect(mSegmentationWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(segmentationOutputArrived(QString)));
	connect(mCenterlineWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(centerlineOutputArrived(QString)));

}

void WirePhantomWidget::activeImageChangedSlot(const QString&)
{
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();

	if (!mUSImageInput->getValue().isEmpty())
		return;

	if (image && image->getModality().contains("US"))
		mUSImageInput->setValue(image->getUid());
}

//void WirePhantomWidget::setColorSlot(QColor color)
//{
//	mCenterlineWidget->setDefaultColor(color);
//	mSegmentationWidget->setDefaultColor(color);
//
//	ssc::MeshPtr mesh;
//
//	mesh = ssc::dataManager()->getMesh(mCenterlineOutput->getValue());
//	if (mesh)
//		mesh->setColor(color);
//	mesh = ssc::dataManager()->getMesh(mSegmentationOutput->getValue());
//	if (mesh)
//		mesh->setColor(color);
//}


WirePhantomWidget::~WirePhantomWidget()
{
}

QString WirePhantomWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Measure US accuracy using the wire phantom.</h3>"
			"<p>"
			"Select a US recording of the wire phantom, then press"
			"the register button to find deviation from the nominal"
			"data. The output is given as a distance from measured"
			"to nominal."
			"</p>"
			"</html>";
}

void WirePhantomWidget::setImageSlot(QString uid)
{
}

//void WirePhantomWidget::resampleOutputArrived(QString uid)
//{
//  mResampleOutput->setValue(uid);
//}

void WirePhantomWidget::segmentationOutputArrived(QString uid)
{
	mSegmentationOutput->setValue(uid);
	this->showData(ssc::dataManager()->getData(uid));
}

void WirePhantomWidget::centerlineOutputArrived(QString uid)
{
	mCenterlineOutput->setValue(uid);
	this->showData(ssc::dataManager()->getData(uid));
}

ssc::MeshPtr WirePhantomWidget::loadNominalCross()
{
	QString nominalCrossFilename = DataLocations::getRootConfigPath()+"/models/wire_phantom_cross_pts.vtk";
	ssc::MeshPtr retval;

	std::map<QString, ssc::MeshPtr> meshes = ssc::dataManager()->getMeshes();
	for (std::map<QString, ssc::MeshPtr>::iterator iter=meshes.begin(); iter!=meshes.end(); ++iter)
		if (iter->first.contains("wire_phantom_cross_pts"))
			retval = iter->second;

	if (!retval)
		retval = boost::shared_dynamic_cast<ssc::Mesh>(patientService()->getPatientData()->importData(nominalCrossFilename));

	if (!retval)
	{
		ssc::messageManager()->sendError(QString("failed to load %s.").arg(nominalCrossFilename));
	}

	retval->setColor(QColor("green"));
	this->showData(retval);

	return retval;
}

void WirePhantomWidget::showData(ssc::DataPtr data)
{
	viewManager()->getViewGroups()[0]->getData()->addData(data);
}

void WirePhantomWidget::measureSlot()
{
	mPendingActions.push_back(boost::bind(&WirePhantomWidget::registration, this));
	QTimer::singleShot(0, this, SLOT(popPendingActions()));
}

void WirePhantomWidget::popPendingActions()
{
	mPendingActions.back()();
	mPendingActions.pop_back();
//	QTimer::singleShot(0, this, SLOT(popPendingActions()));
}

void WirePhantomWidget::registration()
{
	// Verify that a centerline is available:
	//  - if no centerline, run centerline algo on segmentation,
	//  - if no segmentation, run segmentation
	//
	// Set centerline as Registration: moving data.
	// Load nominal cross
	// find center in nominal
	// Execute V2V algo
	// Find transform matrix fMm
	// apply fMm to nominal, return diff
	//

//	std::cout << "using centerline" << mCenterlineOutput->getValue() << std::endl;

	ssc::MeshPtr nominalCross = this->loadNominalCross();
	if (!nominalCross || !mCenterlineOutput->getData())
	{
		ssc::messageManager()->sendError("Missing fixed/moving data. WirePhantom measurement failed.");
		return;
	}

	mManager->setFixedData(nominalCross);
	mManager->setMovingData(mCenterlineOutput->getData());

	this->showData(nominalCross);
	this->showData(mCenterlineOutput->getData());


	SeansVesselReg vesselReg;
	vesselReg.mt_auto_lts = true;
	vesselReg.mt_ltsRatio = 80;
	vesselReg.mt_doOnlyLinear = true;
	QString logPath = patientService()->getPatientData()->getActivePatientFolder() + "/Logs/";
//	vesselReg.setDebugOutput(true);

	bool success = vesselReg.execute(mManager->getMovingData(), mManager->getFixedData(), logPath);
	if (!success)
	{
		ssc::messageManager()->sendWarning("Vessel registration failed.");
		return;
	}

	ssc::Transform3D linearTransform = vesselReg.getLinearResult();
	std::cout << "v2v linear result:\n" << linearTransform << std::endl;

	vesselReg.checkQuality(linearTransform);

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	ssc::Transform3D delta = linearTransform.inv();
	//std::cout << "delta:\n" << delta << std::endl;
	mManager->restart();
	mManager->applyImage2ImageRegistration(delta, "Wire Phantom Measurement");

//		  static Transform3D get_toMfrom(CoordinateSystem from, CoordinateSystem to); ///< to_M_from

	ssc::Vector3D t_delta = linearTransform.matrix().block<3, 1>(0, 3);
	Eigen::AngleAxisd angleAxis = Eigen::AngleAxisd(linearTransform.matrix().block<3, 3>(0, 0));
	double angle = angleAxis.angle();

//	QString qualityText = QString("|t_delta|=%1mm, angle=%2*")
//		.arg(t_delta.length(), 6, 'f', 2)
//		.arg(angle / M_PI * 180.0, 6, 'f', 2);

	ssc::Vector3D cross(134.25, 134.25, 99.50);
	ssc::Vector3D cross_moving = linearTransform.coord(cross);
	double diff = (cross - cross_moving).length();
//	ssc::messageManager()->sendSuccess(QString(""
//		"Registered Wire Phantom. Center point values:\n"
//		"\tnominal: \t%1\n"
//		"\tus:      \t%2\n"
//		"\tshift:   \t%3"
//		).arg(qstring_cast(cross)).arg(qstring_cast(cross_moving)).arg(diff));
	mResults->append(QString(""
		"Shift vector:\t%1\n"
		"Accuracy |v|:\t%2mm\n"
		"Angle:       \t%3*\n"
		"")
		.arg(qstring_cast(cross - cross_moving))
		.arg(diff, 6, 'f', 2)
		.arg(angle / M_PI * 180.0, 6, 'f', 2));

	// add metrics displaying the distance from cross in the nominal and us spaces:
	ssc::Transform3D usMnom = ssc::SpaceHelpers::get_toMfrom(ssc::SpaceHelpers::getD(mManager->getFixedData()), ssc::SpaceHelpers::getD(mManager->getMovingData()));
	ssc::Vector3D cross_us = usMnom.coord(cross);

	ssc::PointMetricPtr p1 = boost::shared_dynamic_cast<ssc::PointMetric>(ssc::dataManager()->getData("cross_nominal"));
	if (!p1)
		p1.reset(new ssc::PointMetric("cross_nominal", "cross_nominal"));
	p1->get_rMd_History()->setParentSpace(nominalCross->getUid());
	p1->setSpace(ssc::SpaceHelpers::getD(nominalCross));
	p1->setCoordinate(cross);
	ssc::dataManager()->loadData(p1);
	this->showData(p1);

	ssc::PointMetricPtr p2 = boost::shared_dynamic_cast<ssc::PointMetric>(ssc::dataManager()->getData("cross_us"));
	if (!p2)
		p2.reset(new ssc::PointMetric("cross_us", "cross_us"));
	p2->get_rMd_History()->setParentSpace(mManager->getMovingData()->getUid());
	p2->setSpace(ssc::SpaceHelpers::getD(mManager->getMovingData()));
	p2->setCoordinate(cross_us);
	ssc::dataManager()->loadData(p2);
	this->showData(p2);

	ssc::DistanceMetricPtr d0 = boost::shared_dynamic_cast<ssc::DistanceMetric>(ssc::dataManager()->getData("accuracy"));
	if (!d0)
		d0.reset(new ssc::DistanceMetric("accuracy", "accuracy"));
	d0->get_rMd_History()->setParentSpace("reference");
	d0->setArgument(0, p1);
	d0->setArgument(1, p2);
	ssc::dataManager()->loadData(d0);
	this->showData(d0);


}


//------------------------------------------------------------------------------
}//namespace cx
