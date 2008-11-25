#include "sscMesh.h"

#include <vtkPolyData.h>

namespace ssc
{

/*
Mesh::Mesh() : 
	mUID("DummyUID"), mName("DummyName"), mVtkPolyData(vtkPolyData::New())
{
}
*/

/*
Mesh::Mesh(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData) : 
	mUID(uid), mName(name), mVtkPolyData(polyData)
{
}
*/

Mesh::Mesh()
{
	mUID = "DummyUID";
	mName = "DummyName";
	mVtkPolyData = vtkPolyData::New();
}

Mesh::Mesh(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData)
{
	mUID = uid;
	mName = name;
	mVtkPolyData = polyData;
}


Mesh::~Mesh()
{
}

//-----
/*
void Mesh::setUID(const std::string& uid)
{
	mUID = uid;
}

void Mesh::setName(const std::string& name)
{
	mName = name;
}
*/

void Mesh::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	mVtkPolyData = polyData;
}

/*
std::string Mesh::getUID() const
{
	return mUID;
}

std::string Mesh::getName() const
{
	return mName;
}
*/

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
