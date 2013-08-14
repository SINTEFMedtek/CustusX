#include "sscMeshHelpers.h"

#include <QColor>

#include <vtkPoints.h>
#include <vtkCellArray.h>

#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscTypeConversions.h"
#include "sscTime.h"

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

std::map<std::string, std::string> getDisplayFriendlyInfo(ssc::MeshPtr mesh)
{
	std::map<std::string, std::string> retval;
	if(!mesh)
		return retval;

	//ssc::mesh
	retval["Filepath"] = mesh->getFilePath().toStdString();
	retval["Coordinate system"] = mesh->getCoordinateSystem().toString().toStdString();
	retval["Name"] = mesh->getName().toStdString();
	retval["Parent space"] = mesh->getParentSpace().toStdString();
	retval["Shading"] = mesh->getShadingOn() ? "on" : "off";
	retval["Space"] = mesh->getSpace().toStdString();
	retval["Type"] = mesh->getType().toStdString();
	retval["Uid"] = mesh->getUid().toStdString();
	retval["rMd"] = string_cast(mesh->get_rMd());
	retval["Backface culling"] = string_cast(mesh->getBackfaceCulling());
	retval["Color"] = mesh->getColor().name().toStdString();
	retval["Frontface culling"] = string_cast(mesh->getFrontfaceCulling());
	retval["Is wireframe"] = string_cast(mesh->getIsWireframe());
	retval["Acquisition time"] = string_cast(mesh->getAcquisitionTime().toString(ssc::timestampSecondsFormatNice()));
	retval["Fiber bundle"] = string_cast(mesh->isFiberBundle());

	//vtkPolyData
	float actualMemorySizeKB = (float)mesh->getVtkPolyData()->GetActualMemorySize();
	retval["Actual memory size"] = string_cast(actualMemorySizeKB/(1024*1024))+" GB, "+string_cast(actualMemorySizeKB/1024)+" MB, "+string_cast(actualMemorySizeKB)+" kB";
	retval["Points"] = string_cast(mesh->getVtkPolyData()->GetNumberOfPoints());
	retval["Lines"] = string_cast(mesh->getVtkPolyData()->GetNumberOfLines());
	retval["Pieces"] = string_cast(mesh->getVtkPolyData()->GetNumberOfPieces());
	retval["Polys"] = string_cast(mesh->getVtkPolyData()->GetNumberOfPolys());
	retval["Strips"] = string_cast(mesh->getVtkPolyData()->GetNumberOfStrips());
	retval["Verts"] = string_cast(mesh->getVtkPolyData()->GetNumberOfVerts());

	return retval;
}

}//namespace ssc

