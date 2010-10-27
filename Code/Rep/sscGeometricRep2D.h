#ifndef SSCGeometricRep2D_H_
#define SSCGeometricRep2D_H_

#include "vtkForwardDeclarations.h"
#include "sscRepImpl.h"
#include "sscVector3D.h"

namespace ssc
{
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

typedef boost::shared_ptr<class GeometricRep2D> GeometricRep2DPtr;

/**Representation for one Mesh.
 * Use this to render geometric polydata in a 2D scene
 * as an intersection between the full polydata and the slice plane.
 * 
 * Used by CustusX.
 */
class GeometricRep2D : public RepImpl
{
  Q_OBJECT
public:
	virtual ~GeometricRep2D();

	static GeometricRep2DPtr New(const QString& uid, const QString& name="");

	virtual QString getType() const { return "ssc::GeometricRep2D"; } ///< gives this reps type
	void setMesh(MeshPtr mesh); ///< sets this reps mesh
	MeshPtr getMesh(); ///< gives this reps mesh
	bool hasMesh(MeshPtr mesh) const; ///< checks if this rep has the give mesh
  void setSliceProxy(ssc::SliceProxyPtr slicer);

protected:
	GeometricRep2D(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPolyDataMapperPtr mMapper;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;

	vtkPolyDataNormalsPtr mNormals;
	vtkCutterPtr mCutter;
	vtkPlanePtr mCutPlane;
	vtkStripperPtr mStripper;
	vtkPolyDataPtr mCutPoly;


	MeshPtr mMesh;
  SliceProxyPtr mSlicer;

private slots:
  void meshChangedSlot();
  void transformChangedSlot();
};

} // namespace ssc

#endif /*SSCGeometricRep2D_H_*/
