
#ifndef SSCUSPROBESECTOR_H_
#define SSCUSPROBESECTOR_H_

#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include "sscProbeSector.h"
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"
#include "sscProbeData.h"

namespace ssc
{
///**
// * \class USProbeSector
// *
// * \date Sep 17, 2010
// * \author Ole Vegard Solberg
// */
//class USProbeSector
//{
//public:
//  USProbeSector(bool threeDimentions);
//  virtual ~USProbeSector();
//  void setSector(ProbeSector sector);
//  void setPosition(const Transform3D& wMt);
//  void createSector();
//  vtkPolyDataPtr getPolyData();
//private:
//  vtkPolyDataPtr mPolyData;
//  vtkPointsPtr mPoints;
//  vtkCellArrayPtr mSides;
//  Transform3D m_wMt;
//  int mType; ///< 0: none, 1: sector, 2: linear
//  double m_d_start; ///< start depth
//  double m_d_end; ///< end depth
//  double mWidth; ///< width in radians or millimeters, depending on type.
//  bool mThreeDimensions; //Is the probe sector going to be shown in 3D?
//};
//typedef boost::shared_ptr<class USProbeSector> USProbeSectorPtr;

}//namespace ssc

#endif /* SSCUSPROBESECTOR_H_ */
