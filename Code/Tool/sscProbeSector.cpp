#include "sscProbeSector.h"

namespace ssc
{

ProbeSector::ProbeSector() : mType(tNONE) 
{
}

ProbeSector::ProbeSector(TYPE type, double depthStart, double depthEnd, double width) : 
	mType(type), mDepthStart(depthStart), mDepthEnd(depthEnd), mWidth(width) 
{
}


}
