#ifndef SSCGEOMETRICREP_H_
#define SSCGEOMETRICREP_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkProperty> vtkPropertyPtr;
typedef	vtkSmartPointer<class vtkActor> vtkActorPtr;

#include "sscRepImpl.h"
#include "sscVector3D.h"

namespace ssc
{
typedef boost::shared_ptr<class Mesh> MeshPtr;

typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;

/**Represenation for one Mesh.
 * Use this to render geometric poly data in a 3D scene.
 * 
 * Used by CustusX.
 */
class GeometricRep : public RepImpl
{
  Q_OBJECT
public:
	virtual ~GeometricRep();

	static GeometricRepPtr New(const QString& uid, const QString& name="");

	virtual QString getType() const { return "ssc::GeometricRep"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh
	
protected:
	GeometricRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPolyDataMapperPtr mMapper;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;

	MeshPtr mMesh;
private slots:
  void meshChangedSlot();
  void transformChangedSlot();
};

} // namespace ssc

#endif /*SSCGEOMETRICREP_H_*/
