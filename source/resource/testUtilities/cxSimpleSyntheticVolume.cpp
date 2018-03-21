/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSimpleSyntheticVolume.h"

#include "cxTypeConversions.h"

namespace cx {

cxSimpleSyntheticVolume::cxSimpleSyntheticVolume(Vector3D bounds)
    :	cxSyntheticVolume(bounds)
{
}

void cxSimpleSyntheticVolume::printInfo() const
{
    std::cout << QString("Volume: Simple, bounds=[%1]")
                 .arg(qstring_cast(mBounds))
                 << std::endl;
}

bool cxSimpleSyntheticVolume::isOnLine(float x,
                      float y,
                      float thickness,
                      int n_lines,
                      int axis) const
{
    float linespacing = mBounds(axis)/n_lines;

    for(int i = 0; i < n_lines; i++)
    {
        if(x > i*linespacing + linespacing/2 && x < i*linespacing + thickness + linespacing/2)
        {
            if(y > 0.0f && y < thickness)
            {
                return true;
            }
        }
    }
    return false;
}

unsigned char cxSimpleSyntheticVolume::evaluate(const cx::Vector3D &p) const
{
    float x = p[0];
    float y = p[1];
    float z = p[2];

    // Let's make a block in the middle of the volume
    if(x > mBounds(0)/3 && x < 2*mBounds(0)/3
       && y > mBounds(1)/3 && y < 2*mBounds(1)/3
       && z > mBounds(2)/3 && z < 2*mBounds(2)/3)
    {
        return 255;
    }

    // A set of thin lines with traversing in each of the directions

    // Z direction
    if(isOnLine(x, y-2.0f, 0.5f, 5, 2))
    {
        return 255;
    }

    if(isOnLine(x, y-3.0f, 0.25f, 5, 2))
    {
        return 255;
    }

    if(isOnLine(x, y-4.0f, 0.125f, 5, 2))
    {
        return 255;
    }
    if(isOnLine(x, y-5.0f, 0.0625f, 5, 2))
    {
        return 255;
    }

    // Y direction
    if(isOnLine(x, z-2.0f, 0.5f, 5, 1))
    {
        return 255;
    }

    if(isOnLine(x, z-3.0f, 0.25f, 5, 1))
    {
        return 255;
    }

    if(isOnLine(x, z-4.0f, 0.125f, 5, 1))
    {
        return 255;
    }
    if(isOnLine(x, z-5.0f, 0.0625f, 5, 1))
    {
        return 255;
    }

    // X direction
    if(isOnLine(z, y-2.0f, 0.5f, 5, 0))
    {
        return 255;
    }

    if(isOnLine(z, y-3.0f, 0.25f, 5, 0))
    {
        return 255;
    }

    if(isOnLine(z, y-4.0f, 0.125f, 5, 0))
    {
        return 255;
    }
    if(isOnLine(z, y-5.0f, 0.0625f, 5, 0))
    {
        return 255;
    }


    // Return nonzero if value is inside region
    else if(x > 0 && x < mBounds(0)
            && y > 0 && y < mBounds(1)
            && z > 0 && z < mBounds(2))
    {
        return 10;
    }
    return 0;
}
} //namespace cx
