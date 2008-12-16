#ifndef SSCGEOMETRICREP_H_
#define SSCGEOMETRICREP_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkProperty> vtkPropertyPtr;
typedef	vtkSmartPointer<class vtkActor> vtkActorPtr;

#include "sscRepImpl.h"
#include "sscMesh.h"

namespace ssc
{
typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;

/**Represenation for one Mesh.
 * Use this to render geometric poly data in a 3D scene.
 */
class GeometricRep : public RepImpl
{
public:
	virtual ~GeometricRep();

	static GeometricRepPtr New(const std::string& uid);

	virtual std::string getType() const { return "ssc::GeometricRep"; }
	void setMesh(MeshPtr mesh);
	//void addImage(ImagePtr image);
	//void removeImage(ImagePtr image);
	bool hasMesh(MeshPtr mesh) const;
protected:
	GeometricRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPolyDataMapperPtr mMapper;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;

	MeshPtr mMesh;
};

} // namespace ssc

#endif /*SSCGEOMETRICREP_H_*/
