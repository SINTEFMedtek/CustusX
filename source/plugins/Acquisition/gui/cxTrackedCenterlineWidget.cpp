#include "cxTrackedCenterlineWidget.h"

#include <QVBoxLayout>
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscMeshHelpers.h"
#include "sscToolRep3D.h"
#include "sscToolTracer.h"
#include "cxViewManager.h"
#include "cxView3D.h"
#include "cxRepManager.h"
#include "cxTrackingDataToVolume.h"
#include "cxPatientData.h"
#include "cxRecordSessionWidget.h"
#include "cxTool.h"
#include "cxPatientService.h"

namespace cx
{
TrackedCenterlineWidget::TrackedCenterlineWidget(AcquisitionDataPtr pluginData, QWidget* parent) :
    TrackedRecordWidget(pluginData, parent, "Tracked centerline")
{
  this->setObjectName("TrackedCenterlineWidget");
  this->setWindowTitle("Tracked Centerline");

//  connect(&mCenterlineAlgorithm, SIGNAL(finished()), this, SLOT(centerlineFinishedSlot()));
//  connect(&mCenterlineAlgorithm, SIGNAL(aboutToStart()), this, SLOT(preprocessResampler()));

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));
  mLayout->addStretch();

  this->checkIfReadySlot();
}

TrackedCenterlineWidget::~TrackedCenterlineWidget()
{}

QString TrackedCenterlineWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tracked Centerline.</h3>"
      "<p><i>Record the tool movement as a centerline.</i></br>"
      "</html>";
}

void TrackedCenterlineWidget::checkIfReadySlot()
{
  if(ssc::toolManager()->isTracking())
  {
    mRecordSessionWidget->setReady(true, "<font color=green>Ready to record!</font>\n");
  }
  else
  {
    mRecordSessionWidget->setReady(false, "<font color=red>Need to start tracking.</font>\n");
  }
}

void TrackedCenterlineWidget::postProcessingSlot(QString sessionId)
{
//  RecordSessionPtr session = mPluginData->getRecordSession(sessionId);
//
//  //get the transforms from the session
//  ssc::TimedTransformMap transforms_prMt = this->getRecording(session);
//  if(transforms_prMt.empty())
//  {
//    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Aborting volume tracking data generation.");
//    return;
//  }
//
//  //visualize the tracked data as a mesh
//  ssc::loadMeshFromToolTransforms(transforms_prMt);
//
//  //convert the transforms into a binary image
//  TrackingDataToVolume converter;
//  int padding = 10;
//  converter.setInput(transforms_prMt, padding);
//  ssc::ImagePtr image_d = converter.getOutput();
//
//  //extract the centerline
//  QString savepath = patientService()->getPatientData()->getActivePatientFolder();
//  mCenterlineAlgorithm.setInput(image_d, savepath);
  mSessionID = sessionId;
//  mCenterlineAlgorithm.execute();
//  mRecordSessionWidget->setReady(false, "<font color=orange>Generating centerline... Please wait!</font>\n");
}

void TrackedCenterlineWidget::preprocessResampler()
{


	RecordSessionPtr session = mPluginData->getRecordSession(mSessionID);

	//get the transforms from the session
	ssc::TimedTransformMap transforms_prMt = this->getRecording(session);
	if(transforms_prMt.empty())
	{
		ssc::messageManager()->sendError("Could not find any tracking data from session "+mSessionID+". Aborting volume tracking data generation.");
		return;
	}

	//visualize the tracked data as a mesh
	ssc::loadMeshFromToolTransforms(transforms_prMt);

	//convert the transforms into a binary image
	TrackingDataToVolume converter;
	int padding = 10;
	converter.setInput(transforms_prMt, padding);
	ssc::ImagePtr image_d = converter.getOutput();

	//extract the centerline
	QString savepath = patientService()->getPatientData()->getActivePatientFolder();
//	mCenterlineAlgorithm.setInput(image_d, savepath);
	mRecordSessionWidget->setReady(false, "<font color=orange>Generating centerline... Please wait!</font>\n");
}


void TrackedCenterlineWidget::centerlineFinishedSlot()
{
  this->checkIfReadySlot();
}

void TrackedCenterlineWidget::startedSlot(QString sessionId)
{
  //show preview of tool path
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();

  View3D* view = viewManager()->get3DView(0,0);
  ssc::ToolRep3DPtr activeRep3D;
  for(; toolIt != tools->end(); ++toolIt)
  {
    activeRep3D = RepManager::findFirstRep<ssc::ToolRep3D>(view->getReps(), toolIt->second);
    if(!activeRep3D)
      continue;
    activeRep3D->getTracer()->clear();
    activeRep3D->getTracer()->start();
  }
}

void TrackedCenterlineWidget::stoppedSlot(bool)
{
  //hide preview of tool path
  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();

  View3D* view = viewManager()->get3DView(0,0);
  ssc::ToolRep3DPtr activeRep3D;
  for(; toolIt != tools->end(); ++toolIt)
  {
    activeRep3D = RepManager::findFirstRep<ssc::ToolRep3D>(view->getReps(), toolIt->second);
    if(!activeRep3D)
      continue;
    if (activeRep3D->getTracer()->isRunning())
    {
      activeRep3D->getTracer()->stop();
      activeRep3D->getTracer()->clear();
    }
  }
}
ssc::TimedTransformMap TrackedCenterlineWidget::getRecording(RecordSessionPtr session)
{
  ssc::TimedTransformMap retval;

  double startTime = session->getStartTime();
  double stopTime = session->getStopTime();

  ToolPtr tool = this->findTool(startTime, stopTime);
  if(!tool)
  {
    ssc::messageManager()->sendWarning("Found no tool with tracking data from the given session.");
    return retval;
  }
  this->setTool(tool);
  retval = tool->getSessionHistory(startTime, stopTime);

  return retval;
}

ToolPtr TrackedCenterlineWidget::findTool(double startTime, double stopTime)
{
  ToolPtr retval;

  ssc::SessionToolHistoryMap toolTransformMap = ssc::toolManager()->getSessionHistory(startTime, stopTime);
  if(toolTransformMap.size() == 1)
  {
    ssc::messageManager()->sendInfo("Found one tool("+toolTransformMap.begin()->first->getName()+") with relevant data.");
    retval = boost::dynamic_pointer_cast<Tool>(toolTransformMap.begin()->first);
  }
  else if(toolTransformMap.size() > 1)
  {
    ssc::messageManager()->sendWarning("Found more than one tool with relevant data, user needs to choose which one to use for tracked centerline extraction.");
    //TODO make the user select which tool they wanna use!!! Pop-up???
    retval = boost::dynamic_pointer_cast<Tool>(toolTransformMap.begin()->first);
    //TODO
  }else if(toolTransformMap.empty())
  {
    ssc::messageManager()->sendWarning("Could not find any session history for given session.");
  }
  return retval;
}
}//namespace cx
