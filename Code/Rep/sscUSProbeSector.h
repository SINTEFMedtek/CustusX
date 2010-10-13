/*
 * sscUSProbeSector.h
 *
 *  Created on: Sep 17, 2010
 *      Author: Ole Vegard Solberg
 */

#ifndef SSCUSPROBESECTOR_H_
#define SSCUSPROBESECTOR_H_

#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include "sscProbeSector.h"
#include "sscTransform3D.h"

typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkCellArray> vtkCellArrayPtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;

namespace ssc
{

class USProbeSector
{
public:
  USProbeSector();
  virtual ~USProbeSector();
  void setSector(ProbeSector sector);
  void setPosition(const Transform3D& wMt);
  void createSector();
  vtkPolyDataPtr getPolyData();
private:
  vtkPolyDataPtr mPolyData;
  vtkPointsPtr mPoints;
  vtkCellArrayPtr mSides;
  Transform3D m_wMt;
  int mType; ///< 0: none, 1: sector, 2: linear
  double m_d_start; ///< start depth
  double m_d_end; ///< end depth
  double mWidth; ///< width in radians or millimeters, depending on type.
};
typedef boost::shared_ptr<class USProbeSector> USProbeSectorPtr;

}//namespace ssc

#endif /* SSCUSPROBESECTOR_H_ */
