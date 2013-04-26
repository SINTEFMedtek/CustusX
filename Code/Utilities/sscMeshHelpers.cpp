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

std::map<std::string, std::string> getDisplayFriendlyInfo(ssc::MeshPtr mesh)
{
	std::map<std::string, std::string> retval;
	if(!mesh)
		return retval;

	//ssc::mesh
	retval["Filepath"] = mesh->getFilePath().toStdString();
	retval["Coordinate system"] = mesh->getCoordinateSystem().toString().toStdString();
//	retval["Image type"] = mesh->getImageType().toStdString();
//	retval["Scalar minimum"] = string_cast(mesh->getMin());
//	retval["Scalar maximum"] = string_cast(mesh->getMax());
//	retval["Range (max - min)"] = string_cast(mesh->getRange());
//	retval["Maximum alpha value"] = string_cast(mesh->getMaxAlphaValue());
//	retval["Modality"] = mesh->getModality().toStdString();
	retval["Name"] = mesh->getName().toStdString();
	retval["Parent space"] = mesh->getParentSpace().toStdString();
	//retval["Registration status"] = enum2string(mesh->getRegistrationStatus()).toStdString();
	retval["Shading"] = mesh->getShadingOn() ? "on" : "off";
	retval["Space"] = mesh->getSpace().toStdString();
	retval["Type"] = mesh->getType().toStdString();
	retval["Uid"] = mesh->getUid().toStdString();

	return retval;
}

}//namespace ssc

