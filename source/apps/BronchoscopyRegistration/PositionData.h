/*
 * PositionData.h
 *
 *  Created on: Jun 21, 2013
 *      Author: ehofstad
 */

#ifndef POSITIONDATA_H_
#define POSITIONDATA_H_

#include <vector>
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscVector3D.h"

typedef std::vector<double> dVector;
typedef std::vector<dVector> dVectors;
typedef std::vector< Eigen::Matrix4d > M4Vector;

namespace cx
{

class PositionData
{
	M4Vector positions;
public:
	PositionData();
	virtual ~PositionData();
	dVectors getPositionData(const char * filename);
	std::pair< M4Vector, Eigen::Matrix4d > loadBronchoscopeTracking(const char * filenameNavigation, const char * filenameDirection);

};

	M4Vector convertToRotMatrix(dVectors positionData);

} /* namespace cx */
#endif /* POSITIONDATA_H_ */
