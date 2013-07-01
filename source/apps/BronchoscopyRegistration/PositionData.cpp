/*
 * PositionData.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: ehofstad
 */

#include "PositionData.h"

namespace cx
{

PositionData::PositionData()
{

}

PositionData::~PositionData()
{

}

dVectors PositionData::getPositionData(const char * filename)
		//Reads position data from .csv file
{
	dVectors positionData;
	std::ifstream inFile (filename);
	std::string line;
    int linenum = 0;
    while (getline (inFile, line))
    {
        linenum++;
        //std::cout << "\nLine #" << linenum << ":" << std::endl;
        std::istringstream linestream(line);
        std::string item;
        int itemnum = 0;
        dVector line;
        while (getline (linestream, item, ','))
        {
            itemnum++;
            if (linenum > 1 && itemnum >= 18 && itemnum <= 24)
            {
            	double number;
            	std::istringstream ( item ) >> number;
            	line.push_back(number);
            }
        }
        positionData.push_back(line);
    }
    return positionData;

}

std::pair< M4Vector, Eigen::Matrix4d > PositionData::loadBronchoscopeTracking(const char * filenameNavigation, const char * filenameDirection)
		//loads bronchoscopy data from csv file and register it to the patient direction
{
	dVectors navigationData;
	dVectors directionData;
	navigationData = getPositionData (filenameNavigation);
	directionData = getPositionData (filenameDirection);

	M4Vector directionPosMatrices = convertToRotMatrix(directionData);
	M4Vector novigationPosMatrices = convertToRotMatrix(navigationData);

	int numberOfPos = directionData.size() - 1;
	Eigen::Matrix4d Tsum(Eigen::Matrix4d::Zero());
	Eigen::Matrix4d Tmean(Eigen::Matrix4d::Zero());
	for ( int index = 0; index < numberOfPos; index++)
	{
		Tsum += directionPosMatrices[index];
	}
	Tmean = Tsum / numberOfPos;

	Eigen::Matrix4d TtoCTcoordinates(Eigen::Matrix4d::Zero());
	TtoCTcoordinates(0,1) = -1;
	TtoCTcoordinates(1,0) = -1;
	TtoCTcoordinates(2,2) = -1;
	TtoCTcoordinates(3,3) = 1;

	TtoCTcoordinates = Tmean * TtoCTcoordinates;

	numberOfPos = novigationPosMatrices.size();
	M4Vector Tnavigation(numberOfPos);
	for ( int index = 0; index <= numberOfPos; index++)
	{
		Tnavigation[index] = TtoCTcoordinates.inverse() * novigationPosMatrices[index];
	}

//low-pass filtering here

	positions = Tnavigation;
	return std::make_pair(Tnavigation , TtoCTcoordinates);

}

M4Vector convertToRotMatrix(dVectors positionData)
		//converts position data to rotation matrix
{
	int numberOfPos = positionData.size() - 1;
	M4Vector posMatrices(numberOfPos - 1);
	for ( int index = 1; index <= numberOfPos; index++)
		{
		Eigen::Quaterniond quat = Eigen::Quaterniond(positionData[index][0] , positionData[index][1] , positionData[index][2] , positionData[index][3]);
		Eigen::Matrix3d rotMatrix(Eigen::Matrix3d::Identity());
		rotMatrix = quat.toRotationMatrix();

		Eigen::Matrix4d Tpos(Eigen::Matrix4d::Identity());
		Tpos(0,0) = rotMatrix(0,0); Tpos(0,1) = rotMatrix(0,1); Tpos(0,2) = rotMatrix(0,2);
		Tpos(1,0) = rotMatrix(1,0); Tpos(1,1) = rotMatrix(1,1); Tpos(1,2) = rotMatrix(1,2);
		Tpos(2,0) = rotMatrix(2,0); Tpos(2,1) = rotMatrix(2,1); Tpos(2,2) = rotMatrix(2,2);
		Tpos(0,3) = positionData[index][4]; Tpos(1,3) = positionData[index][5]; Tpos(2,3) = positionData[index][6];

		posMatrices[index - 1] = Tpos;

		}

return posMatrices;

}

} /* namespace cx */
