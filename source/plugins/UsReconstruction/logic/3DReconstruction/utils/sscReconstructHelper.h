// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCRECONSTRUCTHELPER_H_
#define SSCRECONSTRUCTHELPER_H_

#include <vector>
#include "sscTransform3D.h"
#include "sscReconstructAlgorithm.h"
#include "vtkForwardDeclarations.h"
#include "cxUSReconstructInputData.h"

namespace ssc
{
/**
 *  \class Planes
 *
 *\brief This struct contains three points that defines a plane and
 * the elements of the plane equation: n*r+d = 0
 *
 *  \author Ole Vegard Solberg
 *  \date June 23, 2010
 */
struct Planes
{
	std::vector<ssc::Vector3D> mPoints;
	ssc::Vector3D mNormal;///< Elements of the plane equation n*r+d = 0
	double mDistance;///< Elements of the plane equation n*r+d = 0
};

std::vector<Planes> generate_planes(std::vector<TimedPosition> frameInfo, ImagePtr frameData);

}//namespace ssc
#endif //SSCRECONSTRUCTHELPER_H_
