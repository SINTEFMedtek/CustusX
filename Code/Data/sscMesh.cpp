#include "sscMesh.h"

#include <vtkPolyData.h>

namespace ssc
{
Mesh::Mesh(const std::string& uid, const std::string& name) :
  Data(uid, name), mVtkPolyData(vtkPolyData::New())
{
}
Mesh::Mesh(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData) : 
	Data(uid, name), mVtkPolyData(polyData)
{
}
Mesh::~Mesh()
{
}
void Mesh::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	mVtkPolyData = polyData;
}
vtkPolyDataPtr Mesh::getVtkPolyData()
{
	return mVtkPolyData;
}
//-----
/*
void Mesh::connectRep(const RepWeakPtr& rep)
{
	mReps.insert(rep);
}

void Mesh::disconnectRep(const RepWeakPtr& rep)
{
	mReps.erase(rep);
}
*/

} // namespace ssc
