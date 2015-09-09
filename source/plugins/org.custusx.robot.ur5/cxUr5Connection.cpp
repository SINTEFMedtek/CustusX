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

#include "cxUr5Connection.h"
#include <iostream>
#include <sstream>
#include <QFile>
#include <set>

#include "cxLogger.h"
#include "cxUtilHelpers.h"

namespace cx
{

Ur5Connection::Ur5Connection(QString address, int port)
{
    mIp = address;
    mPort = port;
}

Ur5Connection::Ur5Connection()
{
}

Ur5Connection::~Ur5Connection()
{
}

void Ur5Connection::setAddress(QString address, int port)
{
    mIp = address;
    mPort = port;
}

bool Ur5Connection::isConnectedToRobot()
{
    return (mSocket && mSocket->isConnected());
}

bool Ur5Connection::sendMessage(QString message)
{
    message.append('\n');
    bool ok = this->sendData(message.toStdString().c_str(), message.size());
    if(!ok)
        return false;

    return mSocket->waitForBytesWritten(3000);
}

bool Ur5Connection::waitForUpdate()
{
    mSocket->waitForBytesWritten(3000);
    return mSocket->waitForReadyRead(3000);
}

void Ur5Connection::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

    if(!isPotentialPacket(mSocket->bytesAvailable()))
    {
        mSocket->skip(mSocket->bytesAvailable());
        return;
    }

    mPreviousState = mCurrentState;
    updateCurrentState(mSocket->read(mSocket->bytesAvailable()));
}

bool Ur5Connection::waitForMove()
{
    while(!atTargetPos(mCurrentState))
    {
        waitForUpdate();
    }

    return atTargetPos(mCurrentState);
}

bool Ur5Connection::atTargetPos(Ur5State currentState)
{
    return (currentState.cartAxis-mTargetState.cartAxis).length() < mBlendRadius;
}

void Ur5Connection::runProgramQueue(std::vector<QString> programQueue, std::vector<Ur5State> poseQueue)
{
    for(int i=0;i<programQueue.size();i++)
    {
        mTargetState = poseQueue[i];
        sendMessage(programQueue[i]);
        waitForMove();
    }
    programQueue.clear();
    poseQueue.clear();
    emit(finished());
}

void Ur5Connection::updateCurrentState(QByteArray rawData)
{
    mCurrentState = mMessageDecoder.analyzeRawPacket(rawData);
    if(mCurrentState.updated == false)
        mCurrentState = mPreviousState;

    emit(stateChanged());
}

void Ur5Connection::clearCurrentTCP()
{
    Ur5State clearState(0,0,0,0,0,0);
    sendMessage(mMessageEncoder.set_tcp(clearState));
    waitForUpdate();
}

void Ur5Connection::moveToPlannedOrigo(Ur5State origo)
{
    mTargetState = origo;
    sendMessage(mMessageEncoder.movej(origo,0.3,0.3,0));
    waitForMove();
}

void Ur5Connection::setOrigo(double threshold)
{
    Ur5State zeroState(0,0,0,0,0,0);
    waitForUpdate();

    for(double thres = 1;thres>=threshold;thres= thres/2)
    {
        while(mCurrentState.cartAxis.length()>thres)
        {
            if(mCurrentState.cartAxis[0]*mCurrentState.cartAxis[1]>0) // (-/- case and +/+ case)
            {
                zeroState = incrementPosQuad(zeroState,thres);
            }
            else if(mCurrentState.cartAxis[0]*mCurrentState.cartAxis[1]<0) // (+/- case and -/+)
            {
                zeroState = incrementNegQuad(zeroState,thres);
            }

            sendMessage(mMessageEncoder.set_tcp(zeroState));
            waitForUpdate();
        }
    }

    zeroState.cartAngles=-mCurrentState.cartAngles;

    clearCurrentTCP();

    sendMessage(mMessageEncoder.set_tcp(zeroState));
    waitForUpdate();
}

Ur5State Ur5Connection::incrementPosQuad(Ur5State zeroState, double threshold)
{
    if(mCurrentState.cartAxis[0]>0)
    {
        zeroState.cartAxis[0] -= threshold/2;
    }
    else
    {
        zeroState.cartAxis[0] += threshold/2;
    }

    if(mCurrentState.cartAxis[1]>0)
    {
        zeroState.cartAxis[1] -= threshold/2;
    }
    else
    {
        zeroState.cartAxis[1] += threshold/2;
    }

    if(mCurrentState.cartAxis[2]>0) // Same top/bottom, need to check negative values
    {
        zeroState.cartAxis[2] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[2] -= threshold/2;
    }
    return zeroState;
}

Ur5State Ur5Connection::incrementNegQuad(Ur5State zeroState, double threshold)
{
    if(mCurrentState.cartAxis[0]>0)
    {
        zeroState.cartAxis[0] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[0] -= threshold/2;
    }

    if(mCurrentState.cartAxis[1]>0)
    {
        zeroState.cartAxis[1] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[1] -= threshold/2;
    }

    if(mCurrentState.cartAxis[2]>0) // Same top/bottom
    {
        zeroState.cartAxis[2] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[2] -= threshold/2;
    }
    return zeroState;
}

void Ur5Connection::initializeWorkspace(double threshold, Ur5State origo, bool currentPos)
{
    if(mCurrentState.cartAxis.length()>threshold)
    {
        clearCurrentTCP();
        if(currentPos == false)
            moveToPlannedOrigo(origo);
    }
    setOrigo(threshold);
    emit(finished());
}

bool Ur5Connection::isPotentialPacket(qint64 bytes)
{
    return (bytes == 1460 || bytes == 560 || bytes == 812 || bytes == 1624 || bytes ==1254);
}

Ur5State Ur5Connection::getCurrentState()
{
    return (this->mCurrentState);
}

Ur5State Ur5Connection::getTargetState()
{
    return (this->mCurrentState);
}

Ur5State Ur5Connection::getPreviousState()
{
    return (this->mCurrentState);
}

} // cx
