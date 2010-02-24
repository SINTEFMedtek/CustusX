#ifndef SSCMESH_
#define SSCMESH_

#include <set>

#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;

#include "sscData.h"
#include "sscRep.h"

class QDomNode;
class QDomDocument;

namespace ssc
{

/**Defines a geometric structure in space..?
 * 
 */
class Mesh : public Data
{
public:
	Mesh(const std::string& uid, const std::string& name="");
	Mesh(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData);
	virtual ~Mesh();

	void setVtkPolyData(const vtkPolyDataPtr& polyData);

	virtual vtkPolyDataPtr getVtkPolyData();
  
	void addXml(QDomNode& parentNode); ///< adds xml information about the image and its variabels
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
  
private:
	vtkPolyDataPtr mVtkPolyData;
};

typedef boost::shared_ptr<Mesh> MeshPtr;

} // namespace ssc

#endif /*SSCMESH_*/
