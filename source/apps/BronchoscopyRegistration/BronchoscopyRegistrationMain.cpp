//============================================================================
// Name        : BronchoscopyRegistration.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================


#include <vector>
#include "BronchoscopyRegistration.h"


typedef std::vector<double> dVector;  //endre til eigen vector
typedef std::vector<dVector> dVectors;
typedef std::vector< Eigen::Matrix4d > M4Vector;



int main()
{

	cx::BronchoscopyRegistration* reg = new cx::BronchoscopyRegistration();
	reg->runBronchoscopyRegistration();
	reg->~BronchoscopyRegistration();

	return 0;
}
