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

#include "cxMetricManager.h"
#include "sscDataManager.h"
#include "sscManualTool.h"
#include "cxViewManager.h"
#include "cxViewGroup.h"
#include "cxViewGroupData.h"
#include "cxToolManager.h"
#include <QFile>
#include "sscMessageManager.h"


#include "sscRegistrationTransform.h"
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "cxFrameMetric.h"
#include "cxToolMetric.h"
#include "sscPlaneMetric.h"
#include "cxShapedMetric.h"
#include "sscAngleMetric.h"
#include "cxSphereMetric.h"

namespace cx
{

MetricManager::MetricManager() : QObject(NULL)
{
	connect(toolManager(), SIGNAL(configured()), this, SIGNAL(metricsChanged()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(metricsChanged()));
}

DataMetricPtr MetricManager::getMetric(QString uid)
{
	DataPtr data = dataManager()->getData(uid);
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
	return metric;
}

std::vector<DataMetricPtr> MetricManager::getAllMetrics()
{
	std::vector<DataMetricPtr> retval;
	std::map<QString, DataPtr> all = dataManager()->getData();
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
	Transform3D rMpr = dataManager()->get_rMpr();
	Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ManualToolPtr tool = cxToolManager::getInstance()->getManualTool();
	Vector3D offset = tool->get_prMt().vector(Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	dataManager()->setCenter(p_r);
	Vector3D p0_pr = tool->get_prMt().coord(Vector3D(0, 0, 0));
	tool->set_prMt(createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

PointMetricPtr MetricManager::addPoint(Vector3D point, CoordinateSystem space, QString name)
{
	PointMetricPtr p1(new PointMetric("point%1","point%1"));
  p1->get_rMd_History()->setParentSpace("reference");
	p1->setSpace(space);
	p1->setCoordinate(point);
	dataManager()->loadData(p1);

	viewManager()->getViewGroups()[0]->getData()->addData(p1);
	this->setActiveUid(p1->getUid());

	return p1;
}


void MetricManager::addPointButtonClickedSlot()
{
	this->addPointInDefaultPosition();
}

PointMetricPtr MetricManager::addPointInDefaultPosition()
{
	CoordinateSystem ref = SpaceHelpers::getR();
	Vector3D p_ref = SpaceHelpers::getDominantToolTipPoint(ref, true);
	return this->addPoint(p_ref, ref);
}

void MetricManager::addFrameButtonClickedSlot()
{
  FrameMetricPtr frame(new FrameMetric("frame%1", "frame%1"));
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = SpaceHelpers::getR();
  Transform3D rMt = SpaceHelpers::getDominantToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);

  this->installNewMetric(frame);
}

void MetricManager::addToolButtonClickedSlot()
{
  ToolMetricPtr frame(new ToolMetric("tool%1", "tool%1"));
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = SpaceHelpers::getR();
  Transform3D rMt = SpaceHelpers::getDominantToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);
  frame->setToolName(toolManager()->getDominantTool()->getName());
  frame->setToolOffset(toolManager()->getDominantTool()->getTooltipOffset());

  this->installNewMetric(frame);
}

void MetricManager::addPlaneButtonClickedSlot()
{
  CoordinateSystem ref = SpaceHelpers::getR();
//  Vector3D p_ref = SpaceHelpers::getDominantToolTipPoint(ref, true);

  PlaneMetricPtr p1(new PlaneMetric("plane%1","plane%1"));
  p1->get_rMd_History()->setParentSpace("reference");
  p1->setSpace(ref);

  ToolPtr tool = toolManager()->getDominantTool();
  if (!tool)
  {
	  p1->setCoordinate(Vector3D(0,0,0));
	  p1->setNormal(Vector3D(1,0,0));
  }
  else
  {
	  CoordinateSystem from(csTOOL_OFFSET, tool->getUid());
	  Vector3D point_t = Vector3D(0,0,0);
	  Transform3D rMto = CoordinateSystemHelpers::get_toMfrom(from, ref);

	  p1->setCoordinate(rMto.coord(Vector3D(0,0,0)));
	  p1->setNormal(rMto.vector(Vector3D(0,0,1)));
  }

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
	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
  d0->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
  for (unsigned i=0; i<args.size(); ++i)
	d0->getArguments()->set(i, args[i]);

  this->installNewMetric(d0);
}

void MetricManager::addAngleButtonClickedSlot()
{
	AngleMetricPtr d0(new AngleMetric("angle%1","angle%1"));
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
	SphereMetricPtr d0(new SphereMetric("sphere%1","sphere%1"));
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);

	this->installNewMetric(d0);
}

void MetricManager::addDonutButtonClickedSlot()
{
	DonutMetricPtr d0(new DonutMetric("donut%1","donut%1"));
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);
	d0->getArguments()->set(1, args[1]);

	this->installNewMetric(d0);
}

void MetricManager::installNewMetric(DataMetricPtr metric)
{
	dataManager()->loadData(metric);
	this->setActiveUid(metric->getUid());
	viewManager()->getViewGroups()[0]->getData()->addData(metric);
}

void MetricManager::loadReferencePointsSlot()
{
  ToolPtr refTool = toolManager()->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
	messageManager()->sendDebug("No reference tool, cannot load reference points into the pointsampler");
	return;
  }

  std::map<int, Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
	messageManager()->sendWarning("No referenceppoints in reference tool "+refTool->getName());
	return;
  }

  CoordinateSystem ref = CoordinateSystemHelpers::getR();
  CoordinateSystem sensor = CoordinateSystemHelpers::getS(refTool);

  std::map<int, Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
	Vector3D P_ref = CoordinateSystemHelpers::get_toMfrom(sensor, ref).coord(it->second);
	this->addPoint(P_ref, CoordinateSystem(csREF), "ref%1");
  }
}


void MetricManager::exportMetricsToFile(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	std::map<QString, DataPtr> dataMap = dataManager()->getData();
	std::map<QString, DataPtr>::iterator iter;
	for (iter = dataMap.begin(); iter != dataMap.end(); ++iter)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(iter->second);
		if(metric)
		{
			file.write(metric->getAsSingleLineString().toAscii());
			file.write("\n");
		}
	}
	file.close();
}


} // namespace cx


