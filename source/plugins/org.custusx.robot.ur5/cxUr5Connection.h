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

#ifndef CXUR5CONNECTION_H
#define CXUR5CONNECTION_H

#include "cxSocketConnection.h"
#include "org_custusx_robot_ur5_Export.h"
#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxUr5State.h"
#include "cxUr5Receive.h"
#include "cxUr5RTReceive.h"
#include "cxUr5Transmit.h"


namespace cx
{
/**
 * Class that handles UR5 robot TCP connection.
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Ole Vegard Solberg, SINTEF
 * \author Andreas Østvik
 * \date 2015-06-25
 */
typedef boost::shared_ptr<class Ur5Connection> Ur5ConnectionPtr;

class org_custusx_robot_ur5_EXPORT Ur5Connection : public SocketConnection
{
    Q_OBJECT

public:
    Ur5Connection(QString address, int port);
    Ur5Connection();

    Ur5Receive receiver;
    Ur5RTReceive rtReceiver;
    Ur5Transmit transmitter;
    Ur5State currentState,targetState,jointState;
    QByteArray rawData;

    std::vector<Ur5State> movementQueue;

    double blendRadius = 0.0009;

    void setAddress(QString address, int port);

    bool isConnectedToRobot();
    bool sendMessage(QString message);
    bool waitForMessage();

    void set_rawData(unsigned char* inMessage,qint64 bytes);
    void print_rawData();

    void update_currentState(bool connected = true);

    void initializeWorkspace(double threshold=0.01,Ur5State origo = Ur5State{-0.36,-0.64,0.29,-1.87,-2.50,0},bool currentPos = false);
    void clearCurrentTCP();
    void moveToPlannedOrigo(Ur5State origo);
    void setOrigo(double threshold);

    void incrementPosQuad(Ur5State &state, double threshold);
    void incrementNegQuad(Ur5State &state, double threshold);


    bool waitForMove();
    bool atTargetPos(Ur5State current);

    void set_testData();

    bool runProgramQueue();

private slots:
    virtual void internalDataAvailable();

protected:



};








} // cx

#endif // CXUR5CONNECTION_H
