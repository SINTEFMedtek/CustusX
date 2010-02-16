#include "sscMesh.h"

#include <vtkPolyData.h>

#include <QDomDocument>

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
void Mesh::addXml(QDomNode& parentNode)
{
  //QDomElement datanode = Data::addXml(parentNode);
  
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement meshNode = doc.createElement("mesh");
  parentNode.appendChild(meshNode);
    
  QDomElement uidNode = doc.createElement("uid");
  uidNode.appendChild(doc.createTextNode(mUid.c_str()));
  meshNode.appendChild(uidNode);
  
  QDomElement nameNode = doc.createElement("name");
  nameNode.appendChild(doc.createTextNode(mName.c_str()));
  meshNode.appendChild(nameNode);
  
  QDomElement filePathNode = doc.createElement("filePath");
  filePathNode.appendChild(doc.createTextNode(mFilePath.c_str()));
  meshNode.appendChild(filePathNode);
}
  
void Mesh::parseXml(QDomNode& dataNode)
{
  // image node must be parsed in the data manager to create this Image object
  // Only subnodes are parsed here
  
  if (dataNode.isNull())
    return;
}

} // namespace ssc
