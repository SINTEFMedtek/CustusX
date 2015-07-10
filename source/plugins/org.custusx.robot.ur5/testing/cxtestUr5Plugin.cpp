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
    //REQUIRE(fixture.connection.isConnectedToRobot());

    //QString message("set_pos(p[0,0,0,0,0,0])");
    //QString message("movej(p[0.02,-0.59,0.29,2.94,-1.02,-0.04], a=0.3, v=0.3)");
    //QString message("movej(p[0,-0.6,0.3,3,-1,0], a=0.1, v=0.1,r=0.1)");
    //REQUIRE(fixture.connection.sendMessage(message));


    //QString message2("movej(p[0,-0.4,0.6,2.2,-0.7,0.6], a=0.1, v=0.1)");
    //REQUIRE(fixture.connection.sendMessage(message2));
    //REQUIRE(fixture.connection.waitForMessage());

    //QString message("get_joint_positions()");

    //REQUIRE(fixture.connection.waitForMessage());
    fixture.connection.set_testData();
    fixture.connection.analyze_rawData();
    //fixture.connection.print_rawData();
    fixture.connection.print_cartData();
    //fixture.connection.print_jointData();


//    double v[6] = {0.01,0.01,0.01,0.01,0.01,0.01};
//    REQUIRE(fixture.connection.speedj(v,0.3,3));


//    double axis[3] = {0.3,-0.5,0.3};
//    double angles[3] = {3,-1,0};
//    fixture.connection.movej(axis,angles,0.1,0.1);
//    REQUIRE(fixture.connection.waitForMove());

//    fixture.connection.print_cartData();

//    double axis2[3] = {-0.4,-0.45,0.3};
//    double angles2[3] = {3,-1,0};
//    fixture.connection.movej(axis2,angles2,0.1,0.1);
//    REQUIRE(fixture.connection.waitForMove());

//    fixture.connection.print_cartData();

//    double p1[6] = {0.27,-0.22,0.27,0.19,-2.45,-0.08};
//    double p2[6] = {0.28,-0.28,0.33,1.85,-2.49,0};
//    double t23[6] = {0.29,-0.30,0.375,1.835,-2.49,0.05};
//    double p3[6] = {0.30,-0.32,0.42,1.82,-2.49,0.1};
//    double t34[6] = {0.3025,-0.3375,0.44,1.80,-2.50,0.11};
//    double t34v2[6] = {0.305,-0.355,0.46,1.78,-2.515,0.12};
//    double p4[6] = {0.31,-0.39,0.50,1.74,-2.54,0.14};
//    double t45[6] = {0.28,-0.49,0.485,1.605,-2.625,0.1};
//    double t45v2[6] = {0.265,-0.54,0.4775,1.55,-2.68,0.08};
//    double p5[6] = {0.25,-0.59,0.47,1.47,-2.71,0.06};
//    double t56[6] = {0.235,-0.665,0.405,1.42,-2.755,-0.02};
//    double p6[6] = {0.22,-0.74,0.34,1.37,-2.8,-0.1};
//    double t67[6] = {0.225,-0.765,0.305,1.365,-2.805,-0.11};
//    double p7[6] = {0.23,-0.79,0.27,1.36,-2.81,-0.12};

//    double r = 0.00;
//    double a = 0.1; // 0.1
//    double v = 0.01; // 0.01


//    REQUIRE(fixture.connection.movej(p1,0.5,0.5,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    fixture.connection.movej(p2,0.5,0.5,r);
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(t23,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    fixture.connection.movel(p3,a,v,r);
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(t34,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(t34v2,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(p4,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(t45,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(t45v2,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    fixture.connection.movel(p5,a,v,r);
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    REQUIRE(fixture.connection.movel(t56,a,v,r));
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    fixture.connection.movel(p6,a,v,r);
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    fixture.connection.movel(t67,a,v,r);
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

//    fixture.connection.movel(p7,a,v,r);
//    REQUIRE(fixture.connection.waitForMove());
//    fixture.connection.print_cartData();

    fixture.connection.requestDisconnect();
}

} //cxtest
