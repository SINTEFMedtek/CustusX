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
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"

#include "cxAcquisitionData.h"
#include "sscReconstructManager.h"

namespace cx
{
//------------------------------------------------------------------------------
WirePhantomWidget::WirePhantomWidget(RegistrationManagerPtr regManager, QWidget* parent) :
		RegistrationBaseWidget(regManager, parent, "WirePhantomWidget", "Wire Phantom Test")
{
	mLastRegistration = ssc::Transform3D::Identity();
	connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this,
					SLOT(activeImageChangedSlot(const QString&)));

	mCompositeAlgorithm.reset(new CompositeTimedAlgorithm());
	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mCompositeAlgorithm);

	mLayout = new QVBoxLayout(this);
	mSegmentationWidget = new BinaryThresholdImageFilterWidget(this);
	mCenterlineWidget = new CenterlineWidget(this);

	mUSImageInput = SelectImageStringDataAdapter::New();
	mUSImageInput->setValueName("US Image: ");
	mUSImageInput->setHelp("Select an US volume acquired from the wire phantom.");
    connect(mUSImageInput.get(), SIGNAL(dataChanged(QString)), mSegmentationWidget, SLOT(setImageInputSlot(QString)));

	mSegmentationOutput = SelectImageStringDataAdapter::New();
	mSegmentationOutput->setValueName("Output: ");
    connect(mSegmentationOutput.get(), SIGNAL(dataChanged(QString)), mCenterlineWidget,
			SLOT(setImageInputSlot(QString)));

	mCenterlineOutput = SelectDataStringDataAdapter::New();
	mCenterlineOutput->setValueName("Output: ");
	connect(mCenterlineOutput.get(), SIGNAL(dataChanged(QString)), this, SLOT(setImageSlot(QString)));

	QPushButton* showCrossButton = new QPushButton("Show Cross");
	showCrossButton->setToolTip("Load the centerline description of the wire cross");
	connect(showCrossButton, SIGNAL(clicked()), this, SLOT(loadNominalCross()));

	mCenterlineWidget->setDefaultColor(QColor("red"));
	mSegmentationWidget->setDefaultColor(QColor("red"));

	mMeasureButton = new QPushButton("Execute");
	mMeasureButton->setToolTip("Measure deviation of input volume from nominal wire cross.");
	connect(mMeasureButton, SIGNAL(clicked()), this, SLOT(measureSlot()));

	mCalibrationButton = new QPushButton("Probe calib");
	mCalibrationButton->setToolTip(""
		"Estimate probe calibration sMt\n"
		"based on last accuracy test and\n"
		"the current probe orientation");
	connect(mCalibrationButton, SIGNAL(clicked()), this, SLOT(generate_sMt()));

	QLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mMeasureButton);
	buttonsLayout->addWidget(mCalibrationButton);

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
	mLayout->addWidget(mTimedAlgorithmProgressBar);
	mLayout->addWidget(mResults, 1);
	mLayout->addStretch();

//  connect(mResampleWidget, SIGNAL(outputImageChanged(QString)), this , SLOT(resampleOutputArrived(QString)));
	connect(mSegmentationWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(segmentationOutputArrived(QString)));
	connect(mCenterlineWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(centerlineOutputArrived(QString)));

//	QtSignalAdapters::connect1<void()>(
//		mSegmentationWidget,
//		SIGNAL(inputImageChanged(QString)),
//		boost::bind(&SelectImageStringDataAdapter::setValue, mSegmentationOutput, ""));
//
//	QtSignalAdapters::connect1<void()>(
//		mCenterlineWidget,
//		SIGNAL(inputImageChanged(QString)),
//		boost::bind(&SelectDataStringDataAdapter::setValue, mCenterlineOutput, ""));

	connect(mSegmentationWidget, SIGNAL(inputImageChanged(QString)), this, SLOT(clearSegmentationOutput()));
	connect(mCenterlineWidget,   SIGNAL(inputImageChanged(QString)), this, SLOT(clearCenterlineOutput()));
}


void WirePhantomWidget::clearSegmentationOutput()
{
	mSegmentationOutput->setValue("");
}
void WirePhantomWidget::clearCenterlineOutput()
{
	mCenterlineOutput->setValue("");
}

void WirePhantomWidget::activeImageChangedSlot(const QString&)
{
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();

	if (!mUSImageInput->getValue().isEmpty())
		return;

    // removed: led to setting of preview...
//	if (image && image->getModality().contains("US"))
//		mUSImageInput->setValue(image->getUid());
}


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

void WirePhantomWidget::segmentationOutputArrived(QString uid)
{
	mSegmentationOutput->setValue(uid);
	// only show result if segmentation widget is visible, too much noise otherwise.
	if (mSegmentationWidget->isVisible())
		this->showData(ssc::dataManager()->getData(uid));
}

void WirePhantomWidget::centerlineOutputArrived(QString uid)
{
	std::cout << "WirePhantomWidget::centerlineOutputArrived " << uid << std::endl;
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
	// remove old stuff
	mCompositeAlgorithm->clear();
	disconnect(mCompositeAlgorithm.get(), SIGNAL(finished()), this, SLOT(registration()));

	// add all steps
	if (!mCenterlineOutput->getData())
	{
		if (!mSegmentationOutput->getImage())
		{
			if (!mUSImageInput->getImage())
			{
				ssc::messageManager()->sendWarning("No input selected, measure failed.");
				return;
			}

			mCompositeAlgorithm->append(mSegmentationWidget->getSegmentationAlgorithm());
		}
		mCompositeAlgorithm->append(mCenterlineWidget->getCenterlineAlgorithm());
	}

	connect(mCompositeAlgorithm.get(), SIGNAL(finished()), this, SLOT(registration()));
	mCompositeAlgorithm->execute();
}

/**Compute the centroid of the input mesh.
 */
ssc::Vector3D WirePhantomWidget::findCentroid(ssc::MeshPtr mesh)
{
	vtkPolyDataPtr poly = mesh->getVtkPolyData();
	vtkPointsPtr points = poly->GetPoints();
	int N = points->GetNumberOfPoints();
	if (N==0)
		return ssc::Vector3D(0,0,0);

	ssc::Vector3D acc(0,0,0);
	for (int i = 0; i < N; ++i)
		acc += ssc::Vector3D(points->GetPoint(i));
	return acc/N;
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


	ssc::SeansVesselReg vesselReg;
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
	mLastRegistration = linearTransform;

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	ssc::Transform3D delta = linearTransform.inv();
	mManager->restart();
	mManager->applyImage2ImageRegistration(delta, "Wire Phantom Measurement");


	ssc::Vector3D t_delta = linearTransform.matrix().block<3, 1>(0, 3);
	Eigen::AngleAxisd angleAxis = Eigen::AngleAxisd(linearTransform.matrix().block<3, 3>(0, 0));
	double angle = angleAxis.angle();

	// Compute the centroid of the wire phantom.
	// This should be the wire centre, given that the
	// model is symmetrical.
	ssc::Vector3D cross_r = this->findCentroid(nominalCross);
	//	ssc::Vector3D cross(134.25, 134.25, 99.50); // old hardcoded value: obsole after new measurements
	// should be (CA20121022): <134.212 134.338 100.14>
//	std::cout << "cross2 " << cross2 << std::endl;


	// find transform to probe space t_us, i.e. the middle position from the us acquisition
	std::pair<QString, ssc::Transform3D> probePos = this->getLastProbePosition();
	ssc::Transform3D rMt_us = probePos.second;

	ssc::Vector3D cross_moving_r = linearTransform.coord(cross_r);
	ssc::Vector3D diff_r = (cross_r - cross_moving_r);
	ssc::Vector3D diff_tus = rMt_us.inv().vector(diff_r);

	struct Fmt
	{
		QString operator()(double val)
		{
			return QString("%1").arg(val, 2, 'f', 2);
		}
	};
	Fmt fmt;

	QString result;
	result += QString("Results for: %1\n").arg(mManager->getMovingData()->getName());
	result += QString("Shift vector (r): \t%1\t%2\t%3\n").arg(fmt(diff_r[0])).arg(fmt(diff_r[1])).arg(fmt(diff_r[2]));
	if (!probePos.first.isEmpty())
		result += QString("Shift vector (probe): \t%1\t%2\t%3\t(used tracking data from %4)\n").arg(fmt(diff_tus[0])).arg(fmt(diff_tus[1])).arg(fmt(diff_tus[2])).arg(probePos.first);
	result += QString("Accuracy |v|: \t%1\tmm\n").arg(fmt(diff_r.length()));
	result += QString("Angle: \t%1\t*\n").arg(fmt(angle / M_PI * 180.0));

	mResults->append(result);
	ssc::messageManager()->sendInfo("Wire Phantom Test Results:\n"+result);

	// add metrics displaying the distance from cross in the nominal and us spaces:
	ssc::Transform3D usMnom = ssc::SpaceHelpers::get_toMfrom(ssc::SpaceHelpers::getD(mManager->getFixedData()), ssc::SpaceHelpers::getD(mManager->getMovingData()));
	ssc::Vector3D cross_us = usMnom.coord(cross_r);

	ssc::PointMetricPtr p1 = boost::shared_dynamic_cast<ssc::PointMetric>(ssc::dataManager()->getData("cross_nominal"));
	if (!p1)
		p1.reset(new ssc::PointMetric("cross_nominal", "cross_nominal"));
	p1->get_rMd_History()->setParentSpace(nominalCross->getUid());
	p1->setSpace(ssc::SpaceHelpers::getD(nominalCross));
	p1->setCoordinate(cross_r);
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

/**Find the middle probe position from the latest known us recording.
 * Return this along with the recording filename.
 *
 * Use as a guesstimate for the probe position used during wire measurement.
 *
 */
std::pair<QString, ssc::Transform3D> WirePhantomWidget::getLastProbePosition()
{
	// find transform to probe space t_us, i.e. the middle position from the us acquisition
	ssc::USReconstructInputData usData = mManager->getAcquisitionData()->getReconstructer()->getBaseInputData();
	ssc::Transform3D prMt_us = ssc::Transform3D::Identity();
	if (usData.mPositions.empty())
		return std::make_pair("", ssc::Transform3D::Identity());
	prMt_us = usData.mPositions[usData.mPositions.size()/2].mPos;
	ssc::Transform3D rMt_us = (*ssc::ToolManager::getInstance()->get_rMpr()) * prMt_us;
	return std::make_pair(usData.mFilename, prMt_us);
}

void WirePhantomWidget::generate_sMt()
{
	bool translateOnly = true;


	std::pair<QString, ssc::Transform3D> probePos = this->getLastProbePosition();
	ssc::Transform3D rMt_us = probePos.second;
	if (probePos.first.isEmpty())
	{
		ssc::messageManager()->sendWarning("Cannot find probe position from last recording, aborting calibration test.");
		return;
	}

	ssc::USReconstructInputData usData = mManager->getAcquisitionData()->getReconstructer()->getBaseInputData();
	ssc::ToolPtr probe = ssc::toolManager()->getTool(usData.mProbeUid);
	if (!probe || !probe->hasType(ssc::Tool::TOOL_US_PROBE))
	{
		ssc::messageManager()->sendWarning("Cannot find probe, aborting calibration test.");
		return;
	}

//	ssc::ToolPtr probe = ssc::toolManager()->getDominantTool();
//	if (!probe || !probe->getVisible() || !probe->hasType(ssc::Tool::TOOL_US_PROBE))
//	{
//		ssc::messageManager()->sendWarning("Cannot find visible probe, aborting calibration test.");
//		return;
//	}
	if (!mManager->getMovingData())
	{
		ssc::messageManager()->sendWarning("Cannot find moving data, aborting calibration test.");
		return;
	}


	if (translateOnly)
	{
		ssc::Vector3D cross_r = this->findCentroid(this->loadNominalCross());
		//	ssc::Vector3D cross(134.25, 134.25, 99.50); // old hardcoded value: obsole after new measurements
		// should be (CA20121022): <134.212 134.338 100.14>
	//	std::cout << "cross2 " << cross2 << std::endl;


		// find transform to probe space t_us, i.e. the middle position from the us acquisition
//		std::pair<QString, ssc::Transform3D> probePos = this->getLastProbePosition();
//		ssc::Transform3D rMt_us = probePos.second;

		ssc::Vector3D cross_moving_r = mLastRegistration.coord(cross_r);
		ssc::Vector3D diff_r = (cross_r - cross_moving_r);
		ssc::Vector3D diff_tus = rMt_us.inv().vector(diff_r);

		ssc::Transform3D sMt = probe->getCalibration_sMt();
		ssc::Transform3D sQt; // Q is the new calibration matrix.

		sQt = sMt * ssc::createTransformTranslate(diff_tus);

		ssc::messageManager()->sendInfo(QString(""
				"Calculated new calibration matrix\n"
				"adding only translation "
				"from last accuracy test\n"
				"and raw data %1:\n"
				"%2").arg(probePos.first).arg(qstring_cast(sQt)));
	}
	else
	{
		ssc::Transform3D prMt = rMt_us;
		ssc::Transform3D sMt = probe->getCalibration_sMt();
		ssc::Transform3D prMs = prMt * sMt.inv();
		ssc::Transform3D usMpr = mManager->getMovingData()->get_rMd().inv() * *ssc::toolManager()->get_rMpr();
		ssc::Transform3D nomMus = mLastRegistration.inv();

		ssc::Transform3D sQt; // Q is the new calibration matrix.
		ssc::Transform3D usMs = usMpr*prMs;

		// start with: nomMus * usMpr * prMs * sMt
		// move usMpr*prMs to the left and collect a new sMt from the remains:
		// start with: usMpr * prMs * (usMpr * prMs).inv() * nomMus * usMpr * prMs * sMt

		sQt = usMs.inv() * nomMus * usMs * sMt;

		ssc::messageManager()->sendInfo(QString(""
				"Calculated new calibration matrix\n"
				"from last accuracy test\n"
				"and raw data %1:\n"
				"%2").arg(probePos.first).arg(qstring_cast(sQt)));
	}

}


//------------------------------------------------------------------------------
}//namespace cx
