#ifndef SSCPROBEDATA_H_
#define SSCPROBEDATA_H_

#include <QSize>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
#include "sscTransform3D.h"

#include <boost/shared_ptr.hpp>
#include "sscProbeData.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

typedef boost::shared_ptr<class ProbeSector> ProbeSectorPtr;

/**Definition of characteristics for an Ultrasound Probe.
 */
// class GeometriKlasse
class ProbeSector
{
public:
	ProbeSector();
	//ProbeData(ProbeSector data);
  void setData(ProbeData data);
	
  vtkImageDataPtr getMask();
  vtkPolyDataPtr getSector(); ///< get a polydata representation of the us sector
  vtkPolyDataPtr getSectorLinesOnly(); ///< get a polydata representation of the us sector
  Transform3D get_tMu() const; ///< get transform from image space u to probe tool space t.
  Transform3D get_uMv() const; ///< get transform from inverted image space v (origin in ul corner) to image space u.
  void updateSector();

  ProbeData mData;

  void test();

private:
  bool isInside(Vector3D p_u);
  vtkPolyDataPtr mPolyData; ///< polydata representation of the probe, in space u
	mutable Vector3D mCachedCenter_v; ///< center of beam sector for sector probes. Used in isInside()
};

} // namespace ssc

#endif /*SSCPROBEDATA_H_*/
