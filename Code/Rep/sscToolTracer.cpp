#include "sscToolTracer.h"

#include "vtkImageData.h"
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkProperty.h>
#include <QColor>
#include <vtkMatrix4x4.h>

#include "sscTool.h"
#include "sscBoundingBox3D.h"
#include "sscVolumeHelpers.h"
#include "sscToolManager.h"

namespace ssc
{

ToolTracer::ToolTracer()
{
  mPolyData = vtkPolyDataPtr::New();
  mActor = vtkActorPtr::New();
  mPolyDataMapper = vtkPolyDataMapperPtr::New();

  mPolyDataMapper->SetInput(mPolyData);
  mActor->SetMapper(mPolyDataMapper);

  mProperty = vtkPropertyPtr::New();
  mActor->SetProperty( mProperty );
  mProperty->SetPointSize(4);

  this->setColor(QColor("red"));

  mPoints = vtkPointsPtr::New();
  mLines = vtkCellArrayPtr::New();

  mPolyData->SetPoints(mPoints);
  mPolyData->SetLines(mLines);
  mPolyData->SetVerts(mLines);
}

void ToolTracer::setColor(QColor color)
{
  mActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
}

void ToolTracer::setTool(ToolPtr tool)
{
  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
    disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
  }

  mTool = tool;

  if (mTool)
  {
    connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
    connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
  }
}

vtkPolyDataPtr ToolTracer::getPolyData()
{
  return mPolyData;
}

vtkActorPtr ToolTracer::getActor()
{
  return mActor;
}


void ToolTracer::receiveTransforms(Transform3D prMt, double timestamp)
{
  Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
  Transform3D rMt = rMpr * prMt;

  Vector3D p = rMt.coord(Vector3D(0,0,0));
  mPoints->InsertNextPoint(p.begin());

  // fill cell points for the entire polydata.
  // This is very ineffective, but I have no idea how to update mLines incrementally.
  mLines->Initialize();
  std::vector<vtkIdType> ids(mPoints->GetNumberOfPoints());
  for (unsigned i=0; i<ids.size(); ++i)
    ids[i] = i;
  mLines->InsertNextCell(ids.size(), &(*ids.begin()));

  mPolyData->Modified();
//  mPolyData->Update();
}


}
