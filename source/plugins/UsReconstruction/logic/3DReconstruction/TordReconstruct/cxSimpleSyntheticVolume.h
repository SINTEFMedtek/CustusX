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
		// Return 1 if value is inside region
		else if(x > 0 && x < mDims(0)
		        && y > 0 && y < mDims(0)
		        && z > 0 && z < mDims(0))
		{
			return 1;
		}
		return 0;
	}
	 
};

}
#endif
