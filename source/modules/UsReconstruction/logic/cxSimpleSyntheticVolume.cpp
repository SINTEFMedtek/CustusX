/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
