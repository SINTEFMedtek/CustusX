#ifndef SIMPLESYNTHETICVOLUME_H
#define SIMPLESYNTHETICVOLUME_H

#include "cxSyntheticVolume.h"

namespace cx {

class cxSimpleSyntheticVolume : public cxSyntheticVolume
{
public:
	cxSimpleSyntheticVolume(const Eigen::Array3i& dimensions)
		:	cxSyntheticVolume(dimensions)
	{
	}

	virtual bool isOnLine(float x,
	                      float y,
	                      float thickness,
	                      int n_lines,
	                      int axis) const
	{
		float linespacing = mDims(axis)/n_lines;

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

	virtual unsigned char
	evaluate(const float x,
	         const float y,
	         const float z) const
	{
		// Let's make a block in the middle of the volume
		if(x > mDims(0)/3 && x < 2*mDims(0)/3
		   && y > mDims(1)/3 && y < 2*mDims(1)/3
		   && z > mDims(2)/3 && z < 2*mDims(2)/3)
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
		else if(x > 0 && x < mDims(0)
		        && y > 0 && y < mDims(1)
		        && z > 0 && z < mDims(2))
		{
			return 10;
		}
		return 0;
	}

};

}
#endif
