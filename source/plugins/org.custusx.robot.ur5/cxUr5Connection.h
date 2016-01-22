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
#include "cxUr5MessageDecoder.h"
#include "cxUr5MessageEncoder.h"
#include "cxUr5ProgramEncoder.h"


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
    ~Ur5Connection();

    void setAddress(QString address, int port);
    bool isConnectedToRobot();

    Ur5State getCurrentState();
    Ur5State getTargetState();
    Ur5State getPreviousState();

    bool sendMessage(QString message);

    void updateCurrentState(QByteArray buffer);

    void setProtocol(QString protocolname);

private slots:
    void internalDataAvailable();

signals:
    void stateChanged();

private:
    bool waitForUpdate();
    bool isPotentialPacket(qint64 bytes);

    Ur5MessageEncoder mMessageEncoder;
    Ur5MessageDecoder mMessageDecoder;
    Ur5State mCurrentState,mTargetState,mPreviousState;

    ConnectionInfo info;
};


} // cx

#endif // CXUR5CONNECTION_H
