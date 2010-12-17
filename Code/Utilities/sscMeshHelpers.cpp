#include "sscMeshHelpers.h"

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"

namespace ssc
{
vtkPolyDataPtr polydataFromTransforms(TimedTransformMap transformMap_prMt)
{
  vtkPolyDataPtr retval = vtkPolyDataPtr::New();

  Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();

  vtkPointsPtr points = vtkPointsPtr::New();
  vtkCellArrayPtr lines = vtkCellArrayPtr::New();

  points->Allocate(transformMap_prMt.size());

  TimedTransformMap::iterator mapIter = transformMap_prMt.begin();
  while(mapIter != transformMap_prMt.end())
  {
    Vector3D point_t = Vector3D(0,0,0);

    Transform3D prMt = mapIter->second;
    Transform3D rMt = rMpr * prMt;
    Vector3D p = rMt.coord(point_t);
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

void loadMeshFromToolTransforms(TimedTransformMap transforms_prMt)
{
  //create polydata from positions
  vtkPolyDataPtr centerlinePolydata = ssc::polydataFromTransforms(transforms_prMt);
  QString uid = "tool_positions_mesh_%1";
  QString name = "Tool positions mesh %1";
  MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolydata, uid, name, "Images");
  mesh->setColor(QColor("red"));
  dataManager()->loadData(mesh);
}
}//namespace ssc

