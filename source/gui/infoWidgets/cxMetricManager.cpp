/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxMetricManager.h"
#include "cxManualTool.h"
#include "cxViewGroupData.h"
#include "cxTrackingService.h"
#include <QFile>

#include "cxDataReaderWriter.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxFrameMetric.h"
#include "cxToolMetric.h"
#include "cxPlaneMetric.h"
#include "cxShapedMetric.h"
#include "cxAngleMetric.h"
#include "cxSphereMetric.h"
#include "cxDataFactory.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxLogger.h"

namespace cx
{

MetricManager::MetricManager() : QObject(NULL)
{
	connect(trackingService().get(), &TrackingService::stateChanged, this, &MetricManager::metricsChanged);
	connect(patientService().get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(metricsChanged()));
}

DataMetricPtr MetricManager::getMetric(QString uid)
{
	DataPtr data = patientService()->getData(uid);
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
	return metric;
}

std::vector<DataMetricPtr> MetricManager::getAllMetrics()
{
	std::vector<DataMetricPtr> retval;
	std::map<QString, DataPtr> all = patientService()->getData();
	for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(iter->second);
		if (metric)
			retval.push_back(metric);
	}
	return retval;
}

void MetricManager::setSelection(std::set<QString> selection)
{
	mSelection = selection;
}

void MetricManager::setActiveUid(QString uid)
{
	mActiveLandmark = uid;
	emit activeMetricChanged();
//    this->setModified();
}

void MetricManager::moveToMetric(QString uid)
{
	  DataMetricPtr metric = this->getMetric(uid);
	  if (!metric)
		  return;
	  Vector3D p_r = metric->getRefCoord();;
	  this->setManualToolPosition(p_r);
}

void MetricManager::setManualToolPosition(Vector3D p_r)
{
	Transform3D rMpr = patientService()->get_rMpr();
	Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ToolPtr tool = trackingService()->getManualTool();
	Vector3D offset = tool->get_prMt().vector(Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	patientService()->setCenter(p_r);
	Vector3D p0_pr = tool->get_prMt().coord(Vector3D(0, 0, 0));
	tool->set_prMt(createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

PointMetricPtr MetricManager::addPoint(Vector3D point, CoordinateSystem space, QString uid, QColor color)
{
    PointMetricPtr p1 =	patientService()->createSpecificData<PointMetric>(uid);
	p1->get_rMd_History()->setParentSpace("reference");
	p1->setSpace(space);
	p1->setCoordinate(point);
    p1->setColor(color);
	patientService()->insertData(p1);

	viewService()->getGroup(0)->addData(p1->getUid());
	this->setActiveUid(p1->getUid());

	return p1;
}


void MetricManager::addPointButtonClickedSlot()
{
	this->addPointInDefaultPosition();
}

PointMetricPtr MetricManager::addPointInDefaultPosition()
{
	CoordinateSystem ref = CoordinateSystem::reference();
    QColor color = QColor(240, 170, 255, 255);
    Vector3D p_ref = spaceProvider()->getActiveToolTipPoint(ref, true);

    DataPtr data = patientService()->getData(mActiveLandmark);
    if(!data)

        return this->addPoint(p_ref, ref,"point%1", color);

    PointMetricPtr pointMetric = boost::dynamic_pointer_cast<PointMetric>(data);
    if(pointMetric)
    {
        ref = pointMetric->getSpace();
        p_ref = spaceProvider()->getActiveToolTipPoint(ref, true);
    }

    DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
    if(metric)
        color = metric->getColor();



    return this->addPoint(p_ref, ref,"point%1", color);
}

void MetricManager::addFrameButtonClickedSlot()
{
	FrameMetricPtr frame = patientService()->createSpecificData<FrameMetric>("frame%1");
//	  FrameMetricPtr frame(new FrameMetric("frame%1", "frame%1"));
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = CoordinateSystem::reference();
  Transform3D rMt = spaceProvider()->getActiveToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);

  this->installNewMetric(frame);
}

void MetricManager::addToolButtonClickedSlot()
{
	ToolMetricPtr frame = patientService()->createSpecificData<ToolMetric>("tool%1");
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = CoordinateSystem::reference();
  Transform3D rMt = spaceProvider()->getActiveToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);
  frame->setToolName(trackingService()->getActiveTool()->getName());
  frame->setToolOffset(trackingService()->getActiveTool()->getTooltipOffset());

  this->installNewMetric(frame);
}

void MetricManager::addPlaneButtonClickedSlot()
{
  PlaneMetricPtr p1 = patientService()->createSpecificData<PlaneMetric>("plane%1");
  p1->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(p1->getArguments());
  for (unsigned i=0; i<args.size(); ++i)
	p1->getArguments()->set(i, args[i]);

  this->installNewMetric(p1);
}

/**Starting with a selection of allowed arguments for a new metric,
 * refine them by removing nonselected items, and adding more point
 * metrics if there are too few arguments.
 */
std::vector<DataPtr> MetricManager::refinePointArguments(std::vector<DataPtr> args, unsigned argNo)
{
  // erase non-selected arguments if we have more than enough
  //std::set<QString> selectedUids = this->getSelectedUids();
  for (unsigned i=0; i<args.size();)
  {
	if (args.size() <= argNo)
		break;
	if (!mSelection.count(args[i]->getUid()))
		args.erase(args.begin()+i);
	else
		++i;
  }

  while (args.size() > argNo)
	args.erase(args.begin());

  while (args.size() < argNo)
  {
	  PointMetricPtr p0 = this->addPointInDefaultPosition();
	  args.push_back(p0);
  }

  return args;
}

void MetricManager::addDistanceButtonClickedSlot()
{
	DistanceMetricPtr d0 = patientService()->createSpecificData<DistanceMetric>("distance%1");
//	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
  d0->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
  for (unsigned i=0; i<args.size(); ++i)
	d0->getArguments()->set(i, args[i]);

  this->installNewMetric(d0);
}

void MetricManager::addAngleButtonClickedSlot()
{
	AngleMetricPtr d0 = patientService()->createSpecificData<AngleMetric>("angle%1");
//	AngleMetricPtr d0(new AngleMetric("angle%1","angle%1"));
  d0->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments(), 3);
  d0->getArguments()->set(0, args[0]);
  d0->getArguments()->set(1, args[1]);
  d0->getArguments()->set(2, args[1]);
  d0->getArguments()->set(3, args[2]);

  this->installNewMetric(d0);
}

std::vector<DataPtr> MetricManager::getSpecifiedNumberOfValidArguments(MetricReferenceArgumentListPtr arguments, int numberOfRequiredArguments)
{
	// first try to reuse existing points as distance arguments, otherwise create new ones.

	std::vector<DataMetricPtr> metrics = this->getAllMetrics();

	std::vector<DataPtr> args;
	for (unsigned i=0; i<metrics.size(); ++i)
	{
	  if (arguments->validArgument(metrics[i]))
		  args.push_back(metrics[i]);
	}

	if (numberOfRequiredArguments<0)
		numberOfRequiredArguments = arguments->getCount();
	args = this->refinePointArguments(args, numberOfRequiredArguments);

	return args;
}

void MetricManager::addSphereButtonClickedSlot()
{
	SphereMetricPtr d0 = patientService()->createSpecificData<SphereMetric>("sphere%1");
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);

	this->installNewMetric(d0);
}

void MetricManager::addDonutButtonClickedSlot()
{
	DonutMetricPtr d0 = patientService()->createSpecificData<DonutMetric>("donut%1");
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);
	d0->getArguments()->set(1, args[1]);

	this->installNewMetric(d0);
}

void MetricManager::installNewMetric(DataMetricPtr metric)
{
    DataMetricPtr prevMetric = this->getMetric(mActiveLandmark);
    if(prevMetric)
    {
        metric->setColor(prevMetric->getColor());
    }

	patientService()->insertData(metric);
	this->setActiveUid(metric->getUid());
	viewService()->getGroup(0)->addData(metric->getUid());
}

void MetricManager::loadReferencePointsSlot()
{
  ToolPtr refTool = trackingService()->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
	reportDebug("No reference tool, cannot load reference points into the pointsampler");
	return;
  }

  std::map<int, Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
	reportWarning("No referenceppoints in reference tool "+refTool->getName());
	return;
  }

  CoordinateSystem ref = CoordinateSystem::reference();
  CoordinateSystem sensor = spaceProvider()->getS(refTool);

  std::map<int, Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
	Vector3D P_ref = spaceProvider()->get_toMfrom(sensor, ref).coord(it->second);
	this->addPoint(P_ref, CoordinateSystem(csREF), "ref%1");
  }
}


void MetricManager::exportMetricsToFile(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	std::map<QString, DataPtr> dataMap = patientService()->getData();
	std::map<QString, DataPtr>::iterator iter;
	for (iter = dataMap.begin(); iter != dataMap.end(); ++iter)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(iter->second);
		if(metric)
		{
			file.write(metric->getAsSingleLineString().toLatin1());
			file.write("\n");
		}
	}
	file.close();
}


} // namespace cx


