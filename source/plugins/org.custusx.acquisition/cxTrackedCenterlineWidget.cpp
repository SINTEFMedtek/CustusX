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

#include "cxTrackedCenterlineWidget.h"

#include <QVBoxLayout>
#include "cxTrackingService.h"
#include "cxMeshHelpers.h"
#include "cxToolRep3D.h"
#include "cxToolTracer.h"
//#include "cxTrackingDataToVolume.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxTool.h"
#include "cxLogger.h"
#include "cxView.h"
#include "cxViewService.h"
#include "cxRepContainer.h"
#include "cxAcquisitionService.h"
#include "cxVisServices.h"

namespace cx
{

//TrackedCenterlineWidget::TrackedCenterlineWidget(AcquisitionServicePtr acquisitionService, VisServicesPtr services, QWidget* parent) :
//	TrackedRecordWidget(acquisitionService, parent, "Tracked centerline")
//{
//	this->setObjectName("TrackedCenterlineWidget");
//	this->setWindowTitle("Tracked Centerline");
//	this->setToolTip("Record the tool movement as a centerline");

//	mServices = services;
//	connect(mServices->getToolManager().get(), &TrackingService::stateChanged, this, &TrackedCenterlineWidget::checkIfReadySlot);
//	mLayout->addStretch();

//	this->checkIfReadySlot();
//}

//TrackedCenterlineWidget::~TrackedCenterlineWidget()
//{}

//void TrackedCenterlineWidget::checkIfReadySlot()
//{
//  if(mServices->getToolManager()->getState()>=Tool::tsTRACKING)
//  {
//    mRecordSessionWidget->setReady(true, "<font color=green>Ready to record!</font>\n");
//  }
//  else
//  {
//    mRecordSessionWidget->setReady(false, "<font color=red>Need to start tracking.</font>\n");
//  }
//}

//void TrackedCenterlineWidget::postProcessingSlot(QString sessionId)
//{
////  RecordSessionPtr session = mPluginData->getRecordSession(sessionId);
////
////  //get the transforms from the session
////  TimedTransformMap transforms_prMt = this->getRecording(session);
////  if(transforms_prMt.empty())
////  {
////    reportError("Could not find any tracking data from session "+sessionId+". Aborting volume tracking data generation.");
////    return;
////  }
////
////  //visualize the tracked data as a mesh
////  loadMeshFromToolTransforms(transforms_prMt);
////
////  //convert the transforms into a binary image
////  TrackingDataToVolume converter;
////  int padding = 10;
////  converter.setInput(transforms_prMt, padding);
////  ImagePtr image_d = converter.getOutput();
////
////  //extract the centerline
////  QString savepath = patientService()->getPatientData()->getActivePatientFolder();
////  mCenterlineAlgorithm.setInput(image_d, savepath);
//  mSessionID = sessionId;
////  mCenterlineAlgorithm.execute();
////  mRecordSessionWidget->setReady(false, "<font color=orange>Generating centerline... Please wait!</font>\n");
//}

//void TrackedCenterlineWidget::preprocessResampler()
//{
//	RecordSessionPtr session = mAcquisitionService->getSession(mSessionID);

//	//get the transforms from the session
//	TimedTransformMap transforms_prMt = this->getRecording(session);
//	if(transforms_prMt.empty())
//	{
//		reportError("Could not find any tracking data from session "+mSessionID+". Aborting volume tracking data generation.");
//		return;
//	}

//	//visualize the tracked data as a mesh
//	loadMeshFromToolTransforms(mServices->getPatientService(), transforms_prMt);

//	//convert the transforms into a binary image
////	TrackingDataToVolume converter;
////	int padding = 10;
////	converter.setInput(transforms_prMt, padding);
////	ImagePtr image_d = converter.getOutput();

//	//extract the centerline
////	QString savepath = patientService()->getPatientData()->getActivePatientFolder();
////	mCenterlineAlgorithm.setInput(image_d, savepath);
//	mRecordSessionWidget->setReady(false, "<font color=orange>Generating centerline... Please wait!</font>\n");
//}


//void TrackedCenterlineWidget::centerlineFinishedSlot()
//{
//  this->checkIfReadySlot();
//}

//void TrackedCenterlineWidget::startedSlot(QString sessionId)
//{
//  //show preview of tool path
//  TrackingService::ToolMap tools = mServices->getToolManager()->getTools();
//  TrackingService::ToolMap::iterator toolIt = tools.begin();

//  ToolRep3DPtr activeRep3D;
//  for(; toolIt != tools.end(); ++toolIt)
//  {
//	  activeRep3D = mServices->visualizationService->get3DReps()->findFirst<ToolRep3D>(toolIt->second);
//    if(!activeRep3D)
//      continue;
//    activeRep3D->getTracer()->clear();
//    activeRep3D->getTracer()->start();
//  }
//}

//void TrackedCenterlineWidget::stoppedSlot(bool)
//{
//  //hide preview of tool path
//  TrackingService::ToolMap tools = mServices->getToolManager()->getTools();
//  TrackingService::ToolMap::iterator toolIt = tools.begin();

//  ToolRep3DPtr activeRep3D;
//  for(; toolIt != tools.end(); ++toolIt)
//  {
//	activeRep3D = mServices->visualizationService->get3DReps()->findFirst<ToolRep3D>(toolIt->second);
//    if(!activeRep3D)
//      continue;
//    if (activeRep3D->getTracer()->isRunning())
//    {
//      activeRep3D->getTracer()->stop();
//      activeRep3D->getTracer()->clear();
//    }
//  }
//}
//TimedTransformMap TrackedCenterlineWidget::getRecording(RecordSessionPtr session)
//{
//  TimedTransformMap retval;

//  double startTime = session->getStartTime();
//  double stopTime = session->getStopTime();

//  ToolPtr tool = this->findTool(startTime, stopTime);
//  if(!tool)
//  {
//	reportWarning("Found no tool with tracking data from the given session.");
//    return retval;
//  }
//  this->setTool(tool);
//  retval = tool->getSessionHistory(startTime, stopTime);

//  return retval;
//}

//ToolPtr TrackedCenterlineWidget::findTool(double startTime, double stopTime)
//{
//  ToolPtr retval;

//  SessionToolHistoryMap toolTransformMap = mServices->getToolManager()->getSessionHistory(startTime, stopTime);
//  if(toolTransformMap.size() == 1)
//  {
//	report("Found one tool("+toolTransformMap.begin()->first->getName()+") with relevant data.");
//	retval = toolTransformMap.begin()->first;
//  }
//  else if(toolTransformMap.size() > 1)
//  {
//	reportWarning("Found more than one tool with relevant data, user needs to choose which one to use for tracked centerline extraction.");
//    //TODO make the user select which tool they wanna use!!! Pop-up???
//	retval = toolTransformMap.begin()->first;
//    //TODO
//  }else if(toolTransformMap.empty())
//  {
//	reportWarning("Could not find any session history for given session.");
//  }
//  return retval;
//}
}//namespace cx
