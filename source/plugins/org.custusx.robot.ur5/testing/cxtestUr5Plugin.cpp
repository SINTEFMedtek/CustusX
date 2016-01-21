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
#include "catch.hpp"

#include <iostream>
#include "cxtestUr5TestFixture.h"

namespace cxtest
{

//TEST_CASE("Ur5Plugin: Connect to robot", "[manual][plugins][org.custusx.robot.ur5]")
//{
//    Ur5TestFixture fixture;
//    fixture.mUr5Robot.connectToRobot("169.254.62.100");
//    CHECK(fixture.mUr5Robot.isConnectedToRobot());
//}

//TEST_CASE("Ur5Plugin: Analyze raw data packet and update current state", "[manual][plugins][org.custusx.robot.ur5]")
//{
//    Ur5TestFixture fixture;

//    QByteArray rawData560 = fixture.getTestData(560);
//    fixture.mUr5Connection.updateCurrentState(rawData560);
//    REQUIRE(fixture.mUr5Connection.getCurrentState().updated == true);

//    QByteArray rawData1254 = fixture.getTestData(1254);
//    fixture.mUr5Connection.updateCurrentState(rawData1254);
//    REQUIRE(fixture.mUr5Connection.getCurrentState().updated == true);

//    QByteArray rawData1460 = fixture.getTestData(1460);
//    fixture.mUr5Connection.updateCurrentState(rawData1460);
//    REQUIRE(fixture.mUr5Connection.getCurrentState().updated == true);
//}


//TEST_CASE("Ur5Plugin: Read VTK data", "[manual][plugins][org.custusx.robot.ur5]")
//{
//    Ur5TestFixture fixture;

//    QString path = "C:/line.vtk";
//    fixture.mUr5Robot.openVTKfile(path);

//    QString path2 = "C:/artery_centerline_fixed_2.vtk";
//    fixture.mUr5Robot.openVTKfile(path2);
//}

//TEST_CASE("Ur5Plugin: Compute the jacobian of the robot at given jointPosition", "[manual][plugins][org.custusx.robot.ur5]")
//{
//    Ur5TestFixture fixture;

//    Eigen::RowVectorXd jointPositions(6);
//    jointPositions << 0.7722,-1.7453,1.7070,-1.5739,-1.6277,0.0717;
//    //jointPositions = fixture.mUr5Robot.getCurrentState().jointPosition;

//    Eigen::MatrixXd J(6,6);
//    J = fixture.jacobianUr5(jointPositions);

//    Eigen::RowVectorXd movePose(6);
//    movePose << 0.1,0,0,0,0,0;

//    std::cout << J.determinant() << std::endl;
//    std::cout << jointPositions << std::endl;
//    std::cout << (J.inverse()*movePose.transpose()) << std::endl;
//}

TEST_CASE("Ur5Plugin: Compute positions using forward kinematics", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;
    double threshold = 0.0001;

    Eigen::RowVectorXd jp1(6);
    Eigen::Vector3d operationalPosition, deviation;

    operationalPosition << -96.75/1000, -300.99/1000, 400.55/1000; // Sensor values x = -96.75 mm, y = -300.99 mm, z = 400.55 mm
    jp1 << 0.9019,-2.0358,2.0008,-1.5364,-1.5514,-3.6054; // Sensor values corresponding to above values

    cx::Transform3D computedOperationalPositon = fixture.mUr5Kinematics.forward(jp1);
    deviation = fixture.mUr5Kinematics.T2transl(computedOperationalPositon)-operationalPosition;

    std::cout << Eigen::Affine3d(computedOperationalPositon) << std::endl;

    REQUIRE(deviation(0)<=threshold);
    REQUIRE(deviation(1)<=threshold);
    REQUIRE(deviation(2)<=threshold);
}

TEST_CASE("Ur5Plugin: Dummy test", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;

    Eigen::RowVectorXd jp1(6);

    jp1 << 0.9019,-2.0358,2.0008,-1.5364,-1.5514,-3.6054; // Sensor values

    cx::Transform3D computedOperationalPositon = fixture.mUr5Kinematics.forward(jp1);

    std::cout << fixture.mUr5Kinematics.T2rangles(Eigen::Affine3d(computedOperationalPositon)) << std::endl;
}

} //cxtest
