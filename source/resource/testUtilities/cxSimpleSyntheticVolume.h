/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef SIMPLESYNTHETICVOLUME_H
#define SIMPLESYNTHETICVOLUME_H

#include "cxtestutilities_export.h"

#include "cxSyntheticVolume.h"

namespace cx {

class CXTESTUTILITIES_EXPORT cxSimpleSyntheticVolume : public cxSyntheticVolume
{
public:
    cxSimpleSyntheticVolume(Vector3D bounds);

    virtual void printInfo() const;

	virtual bool isOnLine(float x,
	                      float y,
	                      float thickness,
	                      int n_lines,
                          int axis) const;
    virtual unsigned char evaluate(const cx::Vector3D &p) const;

};

}
#endif
