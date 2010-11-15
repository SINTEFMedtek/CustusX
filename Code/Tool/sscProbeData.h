#ifndef SSCPROBEDATA_H_
#define SSCPROBEDATA_H_

#include <QSize>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
#include "sscTransform3D.h"

#include <boost/shared_ptr.hpp>
#include "sscProbeSector.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

typedef boost::shared_ptr<class ProbeData> ProbeDataPtr;

/**Definition of characteristics for an Ultrasound Probe.
 */
class ProbeData
{
public: 
	enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};	
public:
	ProbeData();
	ProbeData(ProbeSector data);
  void setSector(ProbeSector data);
	
	bool isInside(Vector3D p_u);
  vtkImageDataPtr getMask();
  vtkPolyDataPtr getSector(); ///< get a polydata representation of the us sector
  Transform3D get_tMu() const; ///< get transform from image space u to probe tool space t.
  Transform3D get_uMv() const; ///< get transform from inverted image space v (origin in ul corner) to image space u.
  void updateSector(); ///<update the us sector polydata.

	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR). 

	Vector3D mOrigin_u; ///< probe origin in image space u.
	Vector3D mSpacing;
	QSize mSize;

  void test();

private:
  vtkPolyDataPtr mPolyData; ///< polydata representation of the probe, in space t
	mutable Vector3D mCachedCenter_v; ///< center of beam sector for sector probes. Used in isInside()
};

} // namespace ssc

#endif /*SSCPROBEDATA_H_*/
