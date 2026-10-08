/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxLogger.h"
#include "cxPositionFilter.h"
#include <QDateTime>
#include <cmath>


namespace cxtest
{
class PositionFilterTester : public cx::PositionFilter
{
public:
	PositionFilterTester(unsigned filterStrength, std::vector<cx::TimedPosition> &inputImagePositions) :
		PositionFilter(filterStrength, inputImagePositions)
	{

	}

	void testConvertToQuaternions()
	{
		this->convertToQuaternions();
	}

	void testFilterQuaternionArray()
	{
		this->filterQuaternionArray();
	}

	void testConvertFromQuaternion()
	{
		this->convertFromQuaternion();
	}

	void testFilterPositions()
	{
		this->filterPositions();
	}


};


std::vector<cx::TimedPosition> buildTestArray(int nTestElements)
{
	std::vector<cx::TimedPosition> posVector;
	cx::TimedPosition timedPosition;
	timedPosition.mTime = QDateTime::currentMSecsSinceEpoch(); //Start time
	timedPosition.mPos = cx::Transform3D::Identity();
	Eigen::Matrix4d mMatInitial = timedPosition.mPos.matrix(); //Initial matrix
	for (int i = 0; i < nTestElements; i++)
	{
		timedPosition.mTime = timedPosition.mTime + 50; //Add 50 ms (i.e. 20 Hz frame rate)
		Eigen::Matrix4d mMat = mMatInitial;
		if (i == int(std::floor(nTestElements/2)))
		{
			mMat(2,3) = 10; // Add position jump
		}

		timedPosition.mPos.matrix() = mMat;
		posVector.push_back(timedPosition);
	}
	return posVector;
}

unsigned int comparePositionVectors(std::vector<cx::TimedPosition> &originalImagePositions, std::vector<cx::TimedPosition> &resultImagePositions)
{
	unsigned int testResult = 0;

	unsigned long nElementsOriginal = originalImagePositions.size();
	unsigned long nElementsResult = resultImagePositions.size();
	if (!(nElementsOriginal == nElementsResult))
	{
		//CX_LOG_DEBUG() << "Number of elements in filtered array does not match input.";
		testResult = testResult|1; //Set lsb
	}

	Eigen::Matrix4d sumOriginal;
	sumOriginal.setZero(4,4);
	Eigen::Matrix4d sumResult;
	sumResult.setZero(4,4);

	for (unsigned int i = 0; i < nElementsOriginal; i++)
	{
		if (!(originalImagePositions.at(i).mPos.matrix() == resultImagePositions.at(i).mPos.matrix()))
		{
			//CX_LOG_DEBUG() << "Position array element " << i << " is different";
			testResult = testResult|2; //Set 2nd bit
		}
		sumOriginal = sumOriginal+originalImagePositions.at(i).mPos.matrix();
		sumResult = sumResult+resultImagePositions.at(i).mPos.matrix();
	}

	if (!(sumOriginal == sumResult))
	{
		//CX_LOG_DEBUG() << "Total energy in filtered array is different from original.";
		testResult = testResult|4; //Set 3rd bit
	}

	return testResult;
}


TEST_CASE("PositionFilter: Init","[unit][usreconstruction][synthetic]")
{

	cx::TimedPosition timedPosition;
	timedPosition.mTime = QDateTime::currentMSecsSinceEpoch();
	timedPosition.mPos = cx::Transform3D::Identity();
	unsigned filterStrength = 5;
	std::vector<cx::TimedPosition> posVector;
	posVector.push_back(timedPosition);
	PositionFilterTester positionFilter(filterStrength, posVector);
	REQUIRE(true); //CHECK(true);

}

TEST_CASE("PositionFilter: Filter inactive","[unit][usreconstruction][synthetic]")
{


	unsigned filterStrength = 0; //Filter bypassed
	int nTestElements = 20;

	std::vector<cx::TimedPosition> posVector = buildTestArray(nTestElements);
	std::vector<cx::TimedPosition> posVectorInitial = posVector;
	PositionFilterTester positionFilter(filterStrength, posVector);
	positionFilter.testFilterPositions();

	unsigned int bypassedFilterTestResult = comparePositionVectors(posVectorInitial, posVector);

	REQUIRE(bypassedFilterTestResult == 0); //Filtered array equals input

}

TEST_CASE("PositionFilter: Filter active","[unit][usreconstruction][synthetic]")
{


	unsigned filterStrength = 2; //Filter active
	int nTestElements = 20;

	std::vector<cx::TimedPosition> posVector = buildTestArray(nTestElements);
	std::vector<cx::TimedPosition> posVectorInitial = posVector;
	PositionFilterTester positionFilter(filterStrength, posVector);
	positionFilter.testFilterPositions();

	unsigned int activeFilterTestResult = comparePositionVectors(posVectorInitial, posVector);

	REQUIRE(activeFilterTestResult == 2); //Elements are changed in filtered array, but total energy and # elements unchanged

}


namespace
{
// Poses with constant speed: z position increases 1 mm per pose and a rotation about the z axis increases with
// startAngleDeg + 0.5 deg per pose.
std::vector<cx::TimedPosition> buildMovingArray(int nTestElements, double startAngleDeg)
{
	std::vector<cx::TimedPosition> posVector;
	for (int i = 0; i < nTestElements; i++)
	{
		cx::TimedPosition timedPosition;
		timedPosition.mTime = 50*i;
		timedPosition.mPos = cx::Transform3D::Identity();
		timedPosition.mPos.rotate(Eigen::AngleAxisd((startAngleDeg + 0.5*i)*M_PI/180, Eigen::Vector3d::UnitZ()));
		timedPosition.mPos.pretranslate(Eigen::Vector3d(0, 0, i));
		posVector.push_back(timedPosition);
	}
	return posVector;
}

void requirePositionsUnchanged(const std::vector<cx::TimedPosition> &original, const std::vector<cx::TimedPosition> &result)
{
	REQUIRE(original.size() == result.size());
	for (unsigned i = 0; i < original.size(); i++)
	{
		double translationError = (original[i].mPos.translation() - result[i].mPos.translation()).norm();
		Eigen::Matrix3d rotationDifference = result[i].mPos.rotation() * original[i].mPos.rotation().transpose();
		double rotationError = Eigen::AngleAxisd(rotationDifference).angle();
		INFO("Position " << i);
		CHECK(translationError < 1e-9);
		CHECK(rotationError < 1e-9);
	}
}
} // namespace

TEST_CASE("PositionFilter: Constant speed movement is not changed, including first and last position","[unit][usreconstruction][synthetic]")
{
	// A centered moving average does not change a signal that is linear in time. This must also be true for the
	// first and last positions (no padding with copies of the end positions, which would pull them inwards).
	for (unsigned filterStrength = 1; filterStrength <= 10; filterStrength+=3)
	{
		INFO("Filter strength " << filterStrength);
		std::vector<cx::TimedPosition> posVector = buildMovingArray(50, 0);
		std::vector<cx::TimedPosition> posVectorInitial = posVector;
		PositionFilterTester positionFilter(filterStrength, posVector);
		positionFilter.testFilterPositions();
		requirePositionsUnchanged(posVectorInitial, posVector);
	}
}

TEST_CASE("PositionFilter: Sign change of quaternion between positions","[unit][usreconstruction][synthetic]")
{
	// A rotation sweep where the quaternion from the matrix changes sign between two positions
	// (q and -q are the same rotation). The filtered rotations must still follow the sweep.
	std::vector<cx::TimedPosition> posVector = buildMovingArray(60, 230);
	std::vector<cx::TimedPosition> posVectorInitial = posVector;
	PositionFilterTester positionFilter(5, posVector);
	positionFilter.testFilterPositions();
	requirePositionsUnchanged(posVectorInitial, posVector);
}

} // namespace
