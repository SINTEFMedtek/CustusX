#include "cxRecordBaseWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscTransform3D.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"
#include "cxSegmentation.h"
#include "cxRecordSessionWidget.h"
#include "cxTrackingDataToVolume.h"

namespace cx
{
//----------------------------------------------------------------------------------------------------------------------
RecordBaseWidget::RecordBaseWidget(QWidget* parent, QString description):
    QWidget(parent),
    mLayout(new QVBoxLayout(this)),
    mInfoLabel(new QLabel("Will never be ready... Derive from this class!")),
    mRecordSessionWidget(new RecordSessionWidget(this, description))
{
  this->setObjectName("RecordBaseWidget");
  this->setWindowTitle("Record Base");

  connect(this, SIGNAL(ready(bool)), mRecordSessionWidget, SLOT(setEnabled(bool)));
  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), this, SLOT(postProcessingSlot(QString)));

  mLayout->addWidget(mInfoLabel);
  mLayout->addWidget(mRecordSessionWidget);
}

RecordBaseWidget::~RecordBaseWidget()
{}

void RecordBaseWidget::setWhatsMissingInfo(QString info)
{
  mInfoLabel->setText(info);
}
//----------------------------------------------------------------------------------------------------------------------
TrackedCenterlineWidget::TrackedCenterlineWidget(QWidget* parent) :
    RecordBaseWidget(parent, "Tracked centerline")
{
  this->setObjectName("TrackedCenterlineWidget");
  this->setWindowTitle("Tracked Centerline");

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));

  this->checkIfReadySlot();
}

TrackedCenterlineWidget::~TrackedCenterlineWidget()
{}

void TrackedCenterlineWidget::checkIfReadySlot()
{
  if(ssc::toolManager()->isTracking())
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font>");
    emit ready(true);
  }
  else
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=red>Need to start tracking.</font>");
    emit ready(false);
  }
}

vtkPolyDataPtr TrackedCenterlineWidget::PolydataFromTransforms(ssc::TimedTransformMap transformMap)
{
  vtkPolyDataPtr retval;

  ssc::Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();

  vtkPointsPtr points = vtkPointsPtr::New();
  vtkCellArrayPtr lines = vtkCellArrayPtr::New();

  points->Allocate(transformMap.size());

  ssc::TimedTransformMap::iterator mapIter = transformMap.begin();
  while(mapIter != transformMap.end())
  {
    ssc::Vector3D point_t = ssc::Vector3D(0,0,0);
    //points->InsertNextPoint(mapIter->second.coord(point_t).begin());

    ssc::Transform3D prMt = mapIter->second;
    ssc::Transform3D rMt = rMpr * prMt;
    ssc::Vector3D p = rMt.coord(point_t);
    points->InsertNextPoint(p.begin());

    mapIter++;
  }

  lines->Initialize();
    std::vector<vtkIdType> ids(points->GetNumberOfPoints());
    for (unsigned i=0; i<ids.size(); ++i)
      ids[i] = i;
    lines->InsertNextCell(ids.size(), &(*ids.begin()));

  retval->SetPoints(points);
  retval->SetLines(lines);
  return retval;
}

void TrackedCenterlineWidget::postProcessingSlot(QString sessionId)
{
  //get the transforms from the session
  ssc::TimedTransformMap transforms_prMt = this->getSessionTrackingData(sessionId);
  if(transforms_prMt.empty())
  {
    ssc::messageManager()->sendError("Could not find any tracking data from session "+sessionId+". Aborting centerline extraction.");
    return;
  }

  //Test: create polydata from positions
  vtkPolyDataPtr centerlinePolydata = PolydataFromTransforms(transforms_prMt);

  QString uid = "trackedCenterline_mesh%1";
  QString name = "Tracked centerline mesh %1";
  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolydata, uid, name, "Images");
  mesh->setColor(QColor("red"));
  //mesh->get_rMd_History()->addParentFrame(centerlineImage->getUid());
  ssc::dataManager()->loadData(mesh);


  //Test: remove image converting for now

  //convert the transforms into a binary image
//  TrackingDataToVolume converter;
//  converter.setInput(transforms_prMt);
//  ssc::ImagePtr image_d = converter.getOutput();

  //extract the centerline
//  QString savepath = stateManager()->getPatientData()->getActivePatientFolder();
//  Segmentation segmentation;
//  ssc::ImagePtr centerLineImage_d = segmentation.centerline(image_d, savepath);
}

ssc::TimedTransformMap TrackedCenterlineWidget::getSessionTrackingData(QString sessionId)
{
  ssc::TimedTransformMap retval;
  std::map<ssc::ToolPtr, ssc::TimedTransformMap> toolTransformMap; //TODO toolmanager need to have a function for getting this kind of data?
  ssc::messageManager()->sendDebug("TODO: implement TrackedCenterlineWidget::getSessionTrackingData(QString sessionId)");

  //TODO HACK!!!
  retval = (*ssc::toolManager()->getDominantTool()->getPositionHistory().get());
  //TODO HACK!!!

  if(toolTransformMap.size() == 1)
  {
    return toolTransformMap.begin()->second;
  }
  else if(toolTransformMap.size() > 1)
  {
    //TODO make the user select which tool they wanna use.
    ssc::messageManager()->sendWarning("Found more than one tool with relevant data, user needs to choose which one to use for tracked centerline extraction.");
    ssc::messageManager()->sendDebug("TODO: implement TrackedCenterlineWidget::getSessionTrackingData(QString sessionId)");
  }
  return retval;
}
//----------------------------------------------------------------------------------------------------------------------
USAcqusitionWidget::USAcqusitionWidget(QWidget* parent) :
    RecordBaseWidget(parent, "US Acquisition")
{
  mRTSourceDataAdapter = SelectRTSourceStringDataAdapterPtr(new SelectRTSourceStringDataAdapter());

  this->setObjectName("USAcqusitionWidget");
  this->setWindowTitle("US Acquisition");

  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(checkIfReadySlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(checkIfReadySlot()));

  RecordBaseWidget::mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mRTSourceDataAdapter));

  connect(mRTSourceDataAdapter.get(), SIGNAL(rtSourceChanged()), this, SLOT(rtSourceChangedSlot()));

  this->checkIfReadySlot();
}

USAcqusitionWidget::~USAcqusitionWidget()
{}

void USAcqusitionWidget::checkIfReadySlot()
{
  ssc::messageManager()->sendDebug("TODO: implement USAcqusitionWidget::checkIfReadySlot()");
  if(ssc::toolManager()->isTracking() /*&& mRTSource && mRTSource->isStreaming()*/) //TODO
  {
    RecordBaseWidget::setWhatsMissingInfo("<font color=green>Ready to record!</font>");
    emit ready(true);
  }
  else
  {
    QString whatsMissing("");
    if(!ssc::toolManager()->isTracking())
      whatsMissing.append("<font color=red>Need to start tracking.</font> ");
    if(mRTSource /*&& !mRTSource->isStreaming()*/) //TODO
      whatsMissing.append("<font color=red>Need to start streaming.</font> ");

    RecordBaseWidget::setWhatsMissingInfo(whatsMissing);
    emit ready(false);
  }
}

void USAcqusitionWidget::rtSourceChangedSlot()
{
  if(mRTSource)
  {
    disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
  }
  mRTSource = mRTSourceDataAdapter->getRTSource();
  connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SLOT(checkIfReadySlot()));
}

void USAcqusitionWidget::postProcessingSlot(QString sessionId)
{
  //TODO
  //generate the files needed for reconstruction
}
//----------------------------------------------------------------------------------------------------------------------
}//namespace cx
