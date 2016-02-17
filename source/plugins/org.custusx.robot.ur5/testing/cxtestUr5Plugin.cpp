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
#include <boost/chrono.hpp>
#include "cxUr5State.h"

//#include <chrono>

namespace cxtest
{

TEST_CASE("Ur5Plugin: Analyze raw data packet and update current state", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;

    QByteArray rawData560 = fixture.getByteArrayFromTxt("cxDataFromRobot560.txt");
    fixture.mUr5Connection.updateCurrentState(rawData560);
    REQUIRE(fixture.mUr5Connection.getCurrentState().updated == true);

    QByteArray rawData1254 = fixture.getByteArrayFromTxt("cxDataFromRobot1254.txt");
    fixture.mUr5Connection.updateCurrentState(rawData1254);
    REQUIRE(fixture.mUr5Connection.getCurrentState().updated == true);

    QByteArray rawData1460 = fixture.getByteArrayFromTxt("cxDataFromRobot1460.txt");
    fixture.mUr5Connection.updateCurrentState(rawData1460);
    REQUIRE(fixture.mUr5Connection.getCurrentState().updated == true);
}


TEST_CASE("Ur5Plugin: Read points from .vtk file with polydata and return movementQueue with homogeneous transformation matrices", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;

    QString path = fixture.getTestDataFolderPath()+"line298.vtk";

    std::vector<cx::Ur5MovementInfo> movementQueue = cx::Ur5ProgramEncoder::createMovementQueueFromVTKFile(path);

    REQUIRE(movementQueue.size() == 298);
}

TEST_CASE("Ur5Plugin: Setup program queue", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;

    QString path = fixture.getTestDataFolderPath()+"line298.vtk";

    std::vector<cx::Ur5MovementInfo> movementQueue = cx::Ur5ProgramEncoder::createMovementQueueFromVTKFile(path);
    movementQueue = cx::Ur5ProgramEncoder::addMovementSettings(movementQueue, 0.30, 0.1);
    movementQueue = cx::Ur5ProgramEncoder::addTypeOfMovement(movementQueue, cx::Ur5MovementInfo::movej);

    REQUIRE(movementQueue.at(3).velocity == 0.1);
    REQUIRE(movementQueue.at(3).typeOfMovement == cx::Ur5MovementInfo::movej);
}

TEST_CASE("Ur5Plugin: Compute the jacobian of the robot at given jointPosition", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;

    Eigen::RowVectorXd q1(6);
    q1 << 0.7722,-1.7453,1.7070,-1.5739,-1.6277,0.0717;

    Eigen::RowVectorXd q2(6);
    q2 << 0, 0, 0, 0, 0, 0;

    Eigen::MatrixXd J(6,6);
    J = cx::Ur5Kinematics::jacobian(q2);
}

TEST_CASE("Ur5Plugin: Compute positions using forward kinematics", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;
    double threshold = 0.01;

    Eigen::RowVectorXd q1(6);
    Eigen::Vector3d operationalPosition, deviation;

    operationalPosition << -96.75, -300.99, 400.55; // Sensor values x = -96.75 mm, y = -300.99 mm, z = 400.55 mm
    q1 << 0.9019,-2.0358,2.0008,-1.5364,-1.5514,-3.6054; // Sensor values corresponding to above values

    cx::Transform3D computedOperationalPositon = fixture.mUr5Kinematics.forward(q1);
    deviation = cx::Ur5Kinematics::T2transl(computedOperationalPositon)-operationalPosition;

    //std::cout << Eigen::Affine3d(computedOperationalPositon) << std::endl;

    REQUIRE(deviation(0)<=threshold);
    REQUIRE(deviation(1)<=threshold);
    REQUIRE(deviation(2)<=threshold);
}

TEST_CASE("Ur5Plugin: Compute calibration of prMb and eMt (AX = XB problematique)", "[manual][plugins][org.custusx.robot.ur5]")
{
    Eigen::Matrix4d prMt1, prMt2, prMt3;
    Eigen::Matrix4d bMe1, bMe2, bMe3;

    std::vector<cx::Transform3D> prMt, bMe;

    bMe1 << 0.940208, -0.286317, 0.184478,   79.8462,
            -0.33989, -0.823711, 0.453844, -253.451,
             0.022013,  -0.48941, -0.871776,   381.233,
                    0,         0,         0,         1;

    bMe2 << 0.950807, -0.296844, 0.0886027,   71.1305,
            -0.244507, -0.894724, -0.373744,  -326.797,
             0.190219,  0.333694,  -0.92329,   456.429,
                    0,         0,         0,         1;

    bMe3 << 0.95206,  -0.297485, -0.0713082,    159.371,
            -0.300551,   -0.86616,  -0.399295,   -328.802,
             0.0570201,   0.401585,  -0.914045,    457.184,
                     0,         0,          0,          1;

    prMt1 << 0.401498, 0.79695, -0.451299, 171.994,
             0.91586,    -0.349759,     0.197154,      204.777,
           -0.000724455,    -0.492483,    -0.870322,      264.889,
                     0,            0,            0,            1;

    prMt2 << 0.319993,  0.872892,  0.368327,   352.566,
            0.932267, -0.359359, 0.0417084,   172.098,
            0.168768, 0.330032,  -0.92876,   293.292,
                   0,        0,         0,         1;

    prMt3 << 0.372151,   0.842622,    0.38922,    347.901,
             0.926245,   -0.36416, -0.0972532,    247.515,
            0.0597906,   0.396705,  -0.915997,    322.485,
                    0,          0,          0,          1;

    prMt.push_back(cx::Transform3D(prMt1));
    prMt.push_back(cx::Transform3D(prMt2));
    prMt.push_back(cx::Transform3D(prMt3));

    bMe.push_back(cx::Transform3D(bMe1));
    bMe.push_back(cx::Transform3D(bMe2));
    bMe.push_back(cx::Transform3D(bMe3));

    std::cout << cx::Ur5Kinematics::calibrate_iMk(prMt,bMe) << std::endl;
}

//TEST_CASE("Ur5Plugin: Dummy test", "[manual][plugins][org.custusx.robot.ur5]")
//{
    //Ur5TestFixture fixture;

    //Eigen::RowVectorXd jp1(6);

    //jp1 << 0.9019,-2.0358,2.0008,-1.5364,-1.5514,-3.6054; // Sensor values

    //jp1 << 0,-M_PI/2,0,-M_PI/2,0,0;

//    boost::chrono::high_resolution_clock::time_point t1 =
//        boost::chrono::high_resolution_clock::now();

//    fixture.mUr5Kinematics.jacobian(jp1)*jp1.transpose();

//    boost::chrono::high_resolution_clock::time_point t2 =
//        boost::chrono::high_resolution_clock::now();
//    std::cout << boost::chrono::duration_cast<boost::chrono::nanoseconds>(t2-t1) << "\n";


    //cx::Transform3D computedOperationalPositon = fixture.mUr5Kinematics.forward(jp1);

    //std::cout << fixture.mUr5Kinematics.T2rangles(Eigen::Affine3d(computedOperationalPositon)) << std::endl;

    //std::cout << fixture.mUr5Kinematics.T2OperationalConfiguration(fixture.mUr5Kinematics.forward(jp1)) << std::endl;
//}

} //cxtest
