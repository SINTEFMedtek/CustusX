#ifndef SSCMESH_
#define SSCMESH_

#include <set>
#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;

#include <QColor>
#include "sscData.h"

class QDomNode;
class QDomDocument;

namespace ssc
{

/**Defines a geometric structure in space..?
 * 
 */
class Mesh : public Data
{
  Q_OBJECT
public:
	Mesh(const std::string& uid, const std::string& name="");
	Mesh(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData);
	virtual ~Mesh();

	void setVtkPolyData(const vtkPolyDataPtr& polyData);

	virtual vtkPolyDataPtr getVtkPolyData();
  
	void addXml(QDomNode& parentNode); ///< adds xml information about the image and its variabels
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  
  
  void setColor(const QColor& color);///< Set the color of the mesh
  QColor getColor();///< Get the color of the mesh (Values are range 0 - 255)
  
signals:
  void meshChanged();
private:
	vtkPolyDataPtr mVtkPolyData;
  QColor mColor;
};

typedef boost::shared_ptr<Mesh> MeshPtr;

} // namespace ssc

#endif /*SSCMESH_*/
