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

#include <QPushButton>
#include <QTextEdit>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include "cxDataManager.h"
#include "cxMesh.h"
#include "cxRegistrationManager.h"
#include "cxDataLocations.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxViewManager.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxTool.h"
#include "cxToolManager.h"
#include "cxTypeConversions.h"
#include "cxAcquisitionData.h"
#include "cxReconstructManager.h"
#include "cxPipelineWidget.h"
#include "cxDataReaderWriter.h"
#include "cxDataFactory.h"

#include "cxSmoothingImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxBinaryThresholdImageFilter.h"

#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"
#include "cxReporter.h"

namespace cx
{
//------------------------------------------------------------------------------
WirePhantomWidget::WirePhantomWidget(RegistrationManagerPtr regManager, QWidget* parent) :
        RegistrationBaseWidget(regManager, parent, "WirePhantomWidget", "Wire Phantom Test")
{
    mLastRegistration = Transform3D::Identity();

    // fill the pipeline with filters:
    mPipeline.reset(new Pipeline());
    XmlOptionFile options = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("registration").descend("WirePhantomWidget");
    FilterGroupPtr filters(new FilterGroup(options));
    filters->append(FilterPtr(new SmoothingImageFilter()));
    filters->append(FilterPtr(new BinaryThresholdImageFilter()));
    filters->append(FilterPtr(new BinaryThinningImageFilter3DFilter()));
    mPipeline->initialize(filters);

    mPipeline->getNodes()[0]->setValueName("US Image:");
    mPipeline->getNodes()[0]->setHelp("Select an US volume acquired from the wire phantom.");
    mPipeline->setOption("Color", QVariant(QColor("red")));

    mLayout = new QVBoxLayout(this);

    mPipelineWidget = new PipelineWidget(NULL, mPipeline);

    QPushButton* showCrossButton = new QPushButton("Show Cross");
    showCrossButton->setToolTip("Load the centerline description of the wire cross");
    connect(showCrossButton, SIGNAL(clicked()), this, SLOT(loadNominalCross()));

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
    mLayout->addWidget(mPipelineWidget);
    mLayout->addLayout(buttonsLayout);
    mLayout->addWidget(mResults, 1);
    mLayout->addStretch();
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

MeshPtr WirePhantomWidget::loadNominalCross()
{
    QString nominalCrossFilename = DataLocations::getRootConfigPath()+"/models/wire_phantom_cross_pts.vtk";
    MeshPtr retval;

    std::map<QString, MeshPtr> meshes = dataManager()->getMeshes();
    for (std::map<QString, MeshPtr>::iterator iter=meshes.begin(); iter!=meshes.end(); ++iter)
        if (iter->first.contains("wire_phantom_cross_pts"))
            retval = iter->second;

    if (!retval)
    {
    	QString infoText;
        retval = boost::dynamic_pointer_cast<Mesh>(patientService()->getPatientData()->importData(nominalCrossFilename, infoText));
    }

    if (!retval)
    {
        reportError(QString("failed to load %s.").arg(nominalCrossFilename));
    }

    retval->setColor(QColor("green"));
    this->showData(retval);

    return retval;
}

void WirePhantomWidget::showData(DataPtr data)
{
	viewManager()->getViewGroups()[0]->getData()->addData(data);
}

void WirePhantomWidget::measureSlot()
{
    if (mPipeline->getPipelineTimedAlgorithm()->isRunning())
        return;
    connect(mPipeline->getPipelineTimedAlgorithm().get(), SIGNAL(finished()), this, SLOT(registration()));
    mPipeline->execute();
}

/**Compute the centroid of the input mesh.
 */
Vector3D WirePhantomWidget::findCentroid(MeshPtr mesh)
{
    vtkPolyDataPtr poly = mesh->getVtkPolyData();
    vtkPointsPtr points = poly->GetPoints();
    int N = points->GetNumberOfPoints();
    if (N==0)
        return Vector3D(0,0,0);

    Vector3D acc(0,0,0);
    for (int i = 0; i < N; ++i)
        acc += Vector3D(points->GetPoint(i));
    return acc/N;
}

void WirePhantomWidget::registration()
{
//    // disconnect the connection that started this registration
    disconnect(mPipeline->getPipelineTimedAlgorithm().get(), SIGNAL(finished()), this, SLOT(registration()));

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

    DataPtr measuredCross = mPipeline->getNodes().back()->getData();
    MeshPtr nominalCross = this->loadNominalCross();
    if (!nominalCross || !measuredCross)
    {
        reportError("Missing fixed/moving data. WirePhantom measurement failed.");
        return;
    }

    mManager->setFixedData(nominalCross);
    mManager->setMovingData(measuredCross);

    this->showData(nominalCross);
    this->showData(measuredCross);

    SeansVesselReg vesselReg;
    vesselReg.mt_auto_lts = true;
    vesselReg.mt_ltsRatio = 80;
    vesselReg.mt_doOnlyLinear = true;
    QString logPath = patientService()->getPatientData()->getActivePatientFolder() + "/Logs/";
//	vesselReg.setDebugOutput(true);

    bool success = vesselReg.execute(mManager->getMovingData(), mManager->getFixedData(), logPath);
    if (!success)
    {
        reportWarning("Vessel registration failed.");
        return;
    }

    Transform3D linearTransform = vesselReg.getLinearResult();
    std::cout << "v2v linear result:\n" << linearTransform << std::endl;

    vesselReg.checkQuality(linearTransform);
    mLastRegistration = linearTransform;

    // The registration is performed in space r. Thus, given an old data position rMd, we find the
    // new one as rM'd = Q * rMd, where Q is the inverted registration output.
    // Delta is thus equal to Q:
    Transform3D delta = linearTransform.inv();
    mManager->restart();
    mManager->applyImage2ImageRegistration(delta, "Wire Phantom Measurement");


    Vector3D t_delta = linearTransform.matrix().block<3, 1>(0, 3);
    Eigen::AngleAxisd angleAxis = Eigen::AngleAxisd(linearTransform.matrix().block<3, 3>(0, 0));
    double angle = angleAxis.angle();

    // Compute the centroid of the wire phantom.
    // This should be the wire centre, given that the
    // model is symmetrical.
    Vector3D cross_r = this->findCentroid(nominalCross);
    //	Vector3D cross(134.25, 134.25, 99.50); // old hardcoded value: obsole after new measurements
    // should be (CA20121022): <134.212 134.338 100.14>
//	std::cout << "cross2 " << cross2 << std::endl;


    // find transform to probe space t_us, i.e. the middle position from the us acquisition
    std::pair<QString, Transform3D> probePos = this->getLastProbePosition();
    Transform3D rMt_us = probePos.second;

    Vector3D cross_moving_r = linearTransform.coord(cross_r);
    Vector3D diff_r = (cross_r - cross_moving_r);
    Vector3D diff_tus = rMt_us.inv().vector(diff_r);

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
	report("Wire Phantom Test Results:\n"+result);

    this->showDataMetrics(cross_r);
}

/** Show the nominal and measured cross positions and the line between them as data metrics.
  * The input is the cross in ref space.
  * Note: The two points is shown in the same physical position in the view. Use Registration History
  * to investigate the difference.
  */
void WirePhantomWidget::showDataMetrics(Vector3D cross_r)
{
    // add metrics displaying the distance from cross in the nominal and us spaces:
	Transform3D usMnom = spaceProvider()->get_toMfrom(
				spaceProvider()->getD(mManager->getFixedData()),
				spaceProvider()->getD(mManager->getMovingData()));
    Vector3D cross_us = usMnom.coord(cross_r);

    PointMetricPtr p1 = boost::dynamic_pointer_cast<PointMetric>(dataManager()->getData("cross_nominal"));
    if (!p1)
		p1 = dataManager()->getDataFactory()->createSpecific<PointMetric>("cross_nominal");
//		p1 = PointMetric::create("cross_nominal", "cross_nominal");
    p1->get_rMd_History()->setParentSpace(mManager->getFixedData()->getUid());
	p1->setSpace(spaceProvider()->getD(mManager->getFixedData()));
    p1->setCoordinate(cross_r);
    dataManager()->loadData(p1);
    //this->showData(p1);

    PointMetricPtr p2 = boost::dynamic_pointer_cast<PointMetric>(dataManager()->getData("cross_us"));
    if (!p2)
		p2 = dataManager()->getDataFactory()->createSpecific<PointMetric>("cross_us");
//		p2 = PointMetric::create("cross_us", "cross_us");
    p2->get_rMd_History()->setParentSpace(mManager->getMovingData()->getUid());
	p2->setSpace(spaceProvider()->getD(mManager->getMovingData()));
    p2->setCoordinate(cross_us);
    dataManager()->loadData(p2);
    //this->showData(p2);

    DistanceMetricPtr d0 = boost::dynamic_pointer_cast<DistanceMetric>(dataManager()->getData("accuracy"));
    if (!d0)
		d0 = dataManager()->getDataFactory()->createSpecific<DistanceMetric>("accuracy");
//        d0.reset(new DistanceMetric("accuracy", "accuracy"));
    d0->get_rMd_History()->setParentSpace("reference");
	d0->getArguments()->set(0, p1);
	d0->getArguments()->set(1, p2);
    dataManager()->loadData(d0);
    this->showData(d0);
}

/**Find the middle probe position from the latest known us recording.
 * Return this along with the recording filename.
 *
 * Use as a guesstimate for the probe position used during wire measurement.
 *
 */
std::pair<QString, Transform3D> WirePhantomWidget::getLastProbePosition()
{
    // find transform to probe space t_us, i.e. the middle position from the us acquisition
    USReconstructInputData usData = mManager->getAcquisitionData()->getReconstructer()->getSelectedFileData();
    Transform3D prMt_us = Transform3D::Identity();
    if (usData.mPositions.empty())
        return std::make_pair("", Transform3D::Identity());
    prMt_us = usData.mPositions[usData.mPositions.size()/2].mPos;
	Transform3D rMt_us = dataManager()->get_rMpr() * prMt_us;
    return std::make_pair(usData.mFilename, prMt_us);
}

void WirePhantomWidget::generate_sMt()
{
    bool translateOnly = true;


    std::pair<QString, Transform3D> probePos = this->getLastProbePosition();
    Transform3D rMt_us = probePos.second;
    if (probePos.first.isEmpty())
    {
        reportWarning("Cannot find probe position from last recording, aborting calibration test.");
        return;
    }

    USReconstructInputData usData = mManager->getAcquisitionData()->getReconstructer()->getSelectedFileData();
    ToolPtr probe = toolManager()->getTool(usData.mProbeUid);
    if (!probe || !probe->hasType(Tool::TOOL_US_PROBE))
    {
        reportWarning("Cannot find probe, aborting calibration test.");
        return;
    }

//	ToolPtr probe = toolManager()->getDominantTool();
//	if (!probe || !probe->getVisible() || !probe->hasType(Tool::TOOL_US_PROBE))
//	{
//		reportWarning("Cannot find visible probe, aborting calibration test.");
//		return;
//	}
    if (!mManager->getMovingData())
    {
        reportWarning("Cannot find moving data, aborting calibration test.");
        return;
    }


    if (translateOnly)
    {
        Vector3D cross_r = this->findCentroid(this->loadNominalCross());
        //	Vector3D cross(134.25, 134.25, 99.50); // old hardcoded value: obsole after new measurements
        // should be (CA20121022): <134.212 134.338 100.14>
    //	std::cout << "cross2 " << cross2 << std::endl;


        // find transform to probe space t_us, i.e. the middle position from the us acquisition
//		std::pair<QString, Transform3D> probePos = this->getLastProbePosition();
//		Transform3D rMt_us = probePos.second;

        Vector3D cross_moving_r = mLastRegistration.coord(cross_r);
        Vector3D diff_r = (cross_r - cross_moving_r);
        Vector3D diff_tus = rMt_us.inv().vector(diff_r);

        Transform3D sMt = probe->getCalibration_sMt();
        Transform3D sQt; // Q is the new calibration matrix.

        sQt = sMt * createTransformTranslate(diff_tus);

		report(QString(""
                "Calculated new calibration matrix\n"
                "adding only translation "
                "from last accuracy test\n"
                "and raw data %1:\n"
                "%2").arg(probePos.first).arg(qstring_cast(sQt)));
    }
    else
    {
        Transform3D prMt = rMt_us;
        Transform3D sMt = probe->getCalibration_sMt();
        Transform3D prMs = prMt * sMt.inv();
		Transform3D usMpr = mManager->getMovingData()->get_rMd().inv() * dataManager()->get_rMpr();
        Transform3D nomMus = mLastRegistration.inv();

        Transform3D sQt; // Q is the new calibration matrix.
        Transform3D usMs = usMpr*prMs;

        // start with: nomMus * usMpr * prMs * sMt
        // move usMpr*prMs to the left and collect a new sMt from the remains:
        // start with: usMpr * prMs * (usMpr * prMs).inv() * nomMus * usMpr * prMs * sMt

        sQt = usMs.inv() * nomMus * usMs * sMt;

		report(QString(""
												"Calculated new calibration matrix\n"
												"from last accuracy test\n"
												"and raw data %1.\n"
												"Old calibration matrix sMt:\n"
												"%2\n"
												"New calibration matrix sQt:\n"
												"%3\n").arg(probePos.first).arg(qstring_cast(sMt)).arg(qstring_cast(sQt)));
	}

}


//------------------------------------------------------------------------------
}//namespace cx
