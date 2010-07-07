#include "sscMesh.h"

#include <vtkPolyData.h>
#include <QDomDocument>
#include <QColor>
#include "sscRep.h"
#include "sscTypeConversions.h"

namespace ssc
{
Mesh::Mesh(const std::string& uid, const std::string& name) :
  Data(uid, name), mVtkPolyData(vtkPolyData::New())
{
  mColor = QColor(255, 0, 0, 255);
}
Mesh::Mesh(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData) : 
  Data(uid, name), mVtkPolyData(polyData)
{
  mColor = QColor(255, 0, 0, 255);
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
  
  
  QDomElement colorNode = doc.createElement("color");
  QDomElement subNode = doc.createElement("red");
  subNode.appendChild(doc.createTextNode(string_cast(mColor.red()).c_str()));
  colorNode.appendChild(subNode);
  subNode = doc.createElement("green");
  subNode.appendChild(doc.createTextNode(string_cast(mColor.green()).c_str()));
  colorNode.appendChild(subNode);
  subNode = doc.createElement("blue");
  subNode.appendChild(doc.createTextNode(string_cast(mColor.blue()).c_str()));
  colorNode.appendChild(subNode);
  subNode = doc.createElement("alpha");
  subNode.appendChild(doc.createTextNode(string_cast(mColor.alpha()).c_str()));
  colorNode.appendChild(subNode);
  meshNode.appendChild(colorNode);
}
  
void Mesh::parseXml(QDomNode& dataNode)
{
  // image node must be parsed in the data manager to create this Image object
  // Only subnodes are parsed here
  
  if (dataNode.isNull())
    return;
  
  QDomNode colorNode = dataNode.namedItem("color");
  if (!colorNode.isNull())
  {
    int red   = 255;
    int green = 255;
    int blue  = 255;
    int alpha = 255;
    
    QDomNode node = colorNode.namedItem("red");
    if(!node.isNull())
      red = node.toElement().text().toInt();
    
    node = colorNode.namedItem("green");
    if(!node.isNull())
      green = node.toElement().text().toInt();
    
    node = colorNode.namedItem("blue");
    if(!node.isNull())
      blue = node.toElement().text().toInt();
    
    node = colorNode.namedItem("alpha");
    if(!node.isNull())
      alpha = node.toElement().text().toInt();
    
    mColor = QColor(red, green, blue, alpha);
  }
  
  
}

void Mesh::setColor(const QColor& color)
{
  mColor = color;
  emit meshChanged();
}

QColor Mesh::getColor()
{
  return mColor;
}
  
} // namespace ssc
