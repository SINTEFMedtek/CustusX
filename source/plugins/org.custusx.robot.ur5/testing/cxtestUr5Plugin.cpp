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

TEST_CASE("Ur5Plugin: Connect to robot", "[manual][plugins][org.custusx.robot.ur5]")
{
    //Ur5TestFixture fixture;
    //REQUIRE(fixture.connection.isConnectedToRobot());
    //fixture.connection.requestDisconnect();
}

TEST_CASE("Ur5Plugin: Send message to robot and receive message from robot", "[manual][plugins][org.custusx.robot.ur5]")
{
    Ur5TestFixture fixture;
    REQUIRE(fixture.connection.isConnectedToRobot());

    fixture.connection.update_currentState();
    fixture.connection.print_cartData(fixture.connection.currentState);

    cx::Ur5State p1(0.27,-0.22,0.27,0.19,-2.45,-0.08);
    cx::Ur5State p2(0.28,-0.28,0.33,1.85,-2.49,0);
    cx::Ur5State p3(0.29,-0.30,0.375,1.835,-2.49,0.05);
    cx::Ur5State p4(0.30,-0.32,0.42,1.82,-2.49,0.1);
    cx::Ur5State p5(0.3025,-0.3375,0.44,1.80,-2.50,0.11);
    cx::Ur5State p6(0.305,-0.355,0.46,1.78,-2.515,0.12);
    cx::Ur5State p7(0.31,-0.39,0.50,1.74,-2.54,0.14);
    cx::Ur5State p8(0.28,-0.49,0.485,1.605,-2.625,0.1);
    cx::Ur5State p9(0.265,-0.54,0.4775,1.55,-2.68,0.08);
    cx::Ur5State p10(0.25,-0.59,0.47,1.47,-2.71,0.06);
    cx::Ur5State p11(0.235,-0.665,0.405,1.42,-2.755,-0.02);
    cx::Ur5State p12(0.22,-0.74,0.34,1.37,-2.8,-0.1);
    cx::Ur5State p13(0.225,-0.765,0.305,1.365,-2.805,-0.11);
    cx::Ur5State p14(0.23,-0.79,0.27,1.36,-2.81,-0.12);

    fixture.connection.addToMovementQueue(p1);
    fixture.connection.addToMovementQueue(p2);
    fixture.connection.addToMovementQueue(p3);
    fixture.connection.addToMovementQueue(p4);
    fixture.connection.addToMovementQueue(p5);
    fixture.connection.addToMovementQueue(p6);
    fixture.connection.addToMovementQueue(p7);
    fixture.connection.addToMovementQueue(p8);
    fixture.connection.addToMovementQueue(p9);
    fixture.connection.addToMovementQueue(p10);
    fixture.connection.addToMovementQueue(p11);
    fixture.connection.addToMovementQueue(p12);
    fixture.connection.addToMovementQueue(p13);
    fixture.connection.addToMovementQueue(p14);


    double r = 0.00;
    double a = 0.2; // 0.1
    double v = 0.2; // 0.01

    //fixture.connection.runMovementQueue(fixture.connection.movementQueue,a,v,r);


    fixture.connection.requestDisconnect();
}

} //cxtest
