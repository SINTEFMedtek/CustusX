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
#include <QTcpSocket>
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

Ur5Connection::Ur5Connection(){}

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

bool Ur5Connection::waitForMessage()
{
    mSocket->waitForBytesWritten(3000);
    return mSocket->waitForReadyRead(3000);
}

void Ur5Connection::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

    qint64 maxAvailableBytes = mSocket->bytesAvailable();
    std::cout << maxAvailableBytes << std::endl;

    unsigned char* inMessage = new unsigned char [maxAvailableBytes];

    if(!this->socketReceive(inMessage, maxAvailableBytes))
        return;

    set_rawData(inMessage,maxAvailableBytes);
}

void Ur5Connection::set_rawData(unsigned char* inMessage,qint64 bytes)
{
    rawData = QByteArray(reinterpret_cast<char*>(inMessage),bytes);
}

void Ur5Connection::print_rawData()
{
    std::cout << "Number of bytes: " << rawData.size() << std::endl;
    for(int i=0;i<rawData.size();i++)
    {
        std::cout << rawData.data()[i];
    }
    std::cout << std::endl;
}

bool Ur5Connection::waitForMove()
{
    do
    {
        update_currentState();
    }
    while(!atTargetPos(currentState));
    //receiver.print_cartData(currentState);

    return atTargetPos(currentState);
}

bool Ur5Connection::atTargetPos(Ur5State currentState)
{
    return (currentState.cartAxis-targetState.cartAxis).length() < blendRadius;
}

bool Ur5Connection::runProgramQueue()
{
    for(int i=0;i<transmitter.programQueue.size();i++)
    {
        targetState = transmitter.poseQueue[i];
        sendMessage(transmitter.programQueue[i]);
        waitForMove();
    }
    transmitter.programQueue.clear();
    transmitter.poseQueue.clear();
    return true;
}

void Ur5Connection::update_currentState(bool connected)
{
    if(!connected)
        set_testData();
    else
        waitForMessage();

    if(mPort==30002)
    {
        currentState = receiver.analyze_rawPacket(rawData);
    }
    else if(mPort==30003)
    {
        currentState = rtReceiver.analyze_rawPacket(rawData);
    }
}

void Ur5Connection::clearCurrentTCP()
{
    Ur5State clearState(0,0,0,0,0,0);
    update_currentState();
    sendMessage(transmitter.set_tcp(clearState));
    update_currentState();
}

void Ur5Connection::moveToPlannedOrigo(Ur5State origo)
{
    targetState = origo;
    sendMessage(transmitter.movej(origo,0.3,0.3,0));
    waitForMove();
    update_currentState();
}

void Ur5Connection::incrementPosQuad(Ur5State &zeroState, double threshold)
{
    if(currentState.cartAxis[0]>0)
    {
        zeroState.cartAxis[0] -= threshold/2;
    }
    else
    {
        zeroState.cartAxis[0] += threshold/2;
    }

    if(currentState.cartAxis[1]>0)
    {
        zeroState.cartAxis[1] -= threshold/2;
    }
    else
    {
        zeroState.cartAxis[1] += threshold/2;
    }

    if(currentState.cartAxis[2]>0) // Same top/bottom, need to check negative values
    {
        zeroState.cartAxis[2] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[2] -= threshold/2;
    }
}

void Ur5Connection::incrementNegQuad(Ur5State &zeroState, double threshold)
{
    if(currentState.cartAxis[0]>0)
    {
        zeroState.cartAxis[0] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[0] -= threshold/2;
    }

    if(currentState.cartAxis[1]>0)
    {
        zeroState.cartAxis[1] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[1] -= threshold/2;
    }

    if(currentState.cartAxis[2]>0) // Same top/bottom
    {
        zeroState.cartAxis[2] += threshold/2;
    }
    else
    {
        zeroState.cartAxis[2] -= threshold/2;
    }
}

void Ur5Connection::setOrigo(double threshold)
{
    Ur5State zeroState(0,0,0,0,0,0);
    update_currentState();

    for(double thres = 1;thres>=threshold;thres= thres/2)
    {
        while(currentState.cartAxis.length()>thres)
        {
            if(currentState.cartAxis[0]*currentState.cartAxis[1]>0) // (-/- case and +/+ case)
            {
                incrementPosQuad(zeroState,thres);
            }
            else if(currentState.cartAxis[0]*currentState.cartAxis[1]<0) // (+/- case and -/+)
            {
                incrementNegQuad(zeroState,thres);
            }

            sendMessage(transmitter.set_tcp(zeroState));
            update_currentState();
            std::cout << currentState.cartAxis << std::endl;
        }
    }

    zeroState.cartAngles=-currentState.cartAngles;

    clearCurrentTCP();

    sendMessage(transmitter.set_tcp(zeroState));
    update_currentState();
    receiver.print_cartData(currentState);
}

void Ur5Connection::initializeWorkspace(double threshold, Ur5State origo, bool currentPos)
{
    if(currentState.cartAxis.length()>threshold)
    {
        clearCurrentTCP();
        if(currentPos == false)
            moveToPlannedOrigo(origo);
    }

    setOrigo(threshold);
}

void Ur5Connection::set_testData() // Test data, may be removed later
{
    //rawData = QByteArray::fromHex("000004e6100000001d000000000000070b4001010100000000003ff0000000000000000000fb0140129cbfa3ff339a40129cbaa30d842e00000000000000003d8e53fd423e666741e000004253999afdbff903c903a50f28bff903da0998505c00000000000000004001d87e423e666741e66667425b999afdbff39543291650f0bff3952dca3e8fdd00000000000000003fc970ad423e666741e000004253999afdbfff1bba48ed7f84bfff1bc656ae4b4800000000000000003e832d8e423e00004204cccd425f999afd3ffa063d748b83743ffa06475f9d0b1c00000000000000003e832d8e423e6667420ccccd426c6667fdbff947eb52e92b58bff947e533bc678500000000000000003e177ab3423f999a42113333427b999afd0000003504bfc0d5af562ce9aabfdcda179aa84d0b3fe21e88537014ebc0010712885eeef94001e95d0ce34aa1bfa5d57f65e8a5cd000000e1053c6144ff1c65307f0c75ce3fa23968ff7d5bc39f3658b9ff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bfdb333333333333bfd91a9fbe76c8b40000000000000000000000000000000000000000000000003fb6d31fcd24e161000000000000000000000000000000003fbbf141205bc01a3fb83afb7e90ff973fb5119ce075f6fd3ff921fb54524550000000000000000000000000000000003ff921fb54524550bff921fb545245500000000000000000000000000000003509bfec3de20a35cbb9c02254dd7f203b37c00878db7c24eb7ebfed005a97a92bd4c002f6388e6a1839bff63172941fc304000000400300000000000000000000000000003f4401401401401400000000000000000000000000000000000041f5999a424133333f45fa0000000000080200000000250200003f927cca98db97f73f9018cae1c8068542353333003bc49ba6424c0000fd000001d506c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d184009333333333333402e0000000000004009333333333333402e0000000000004009333333333333402e000000000000400999999999999a4039000000000000400999999999999a4039000000000000400999999999999a40390000000000003ff0c152382d73653ff657184ae744873fd00000000000003ff33333333333333fd00000000000000000000000000000bfdb333333333333bfd91a9fbe76c8b40000000000000000000000000000000000000000000000003fb6d31fcd24e161000000000000000000000000000000003fbbf141205bc01a3fb83afb7e90ff973fb5119ce075f6fd3ff921fb54524550000000000000000000000000000000003ff921fb54524550bff921fb545245500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000050000000200000001000000010000000200000002000000020000000200000002000000020000003d070000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003f752e79388a2f5900000007080001");
    rawData = QByteArray::fromHex("0000032c408dbcbc6a7ef9db3ff49c2971d2c8f0bff82a94174916953fefff87fd48e5e0bfee259851062756bff940ba69d50d043feda1ddfbafceb6000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bc487d714d37c9c6bff4ea94ed41a933bff2178e7059ac9bbf71ad29e32a52ce3f970537c8cc74b1bf6f26491517ffe3bc83701b33cb8000c030772ee61ae4eec02c58e78181e636bfa488d0610de3fe3fcad6920007e630bfa2413bf5beddc53ff49c3d3738ec82bff82a7c106621293fefff5530101ee0bfee25765647dc4cbff940a96c75e7803feda1bc00f183ac0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003fc339cbf301bf39bffb8c2067a6cd37bff9652ddf8dc52f3fb8988a8c843d673f82bd5d5edea8b03f82bd5d5edea8b0bfc1a537e4c7b321c00670b92b4be1054020c532dc640e98000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c00c4fe9d02ca29e3ffed7d7010d813dc0322d19a2c07ecf4023446f5669aaadc01970b069a927f3c008df07bdb0d1423fd32aef5386a1113fcd05deec1057443fda0c00f3f66e4c3fba7e5e24f6e999bfa7adc1df2b64123fb33890ad929f56000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040383333400000004038e666600000004038e66660000000403c000000000000403c000000000000403d0000000000003fb040af1cb89d6b40200000000000000000000000000000406fa00000000000406fa00000000000406fa00000000000406fa00000000000406fa00000000000406fa000000000000000032c408dbccccccccccd3ff49c2971d2c8f0bff82a94174916953fefff87fd48e5e0bfee259851062756bff940ba69d50d043feda1ddfbafceb6000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bc487d714d37c9c6bff4ea94ed41a933bff2178e7059ac9bbf71ad29e32a52ce3f970537c8cc74b1bf6f26491517ffe3bc83701b33cb8000c030772ee61ae4eec02c58e78181e636bfa488d0610de3fe3fcad6920007e630bfa2413bf5beddc53ff49c3d3738ec82bff82a7c106621293fefff5530101ee0bfee25765647dc4cbff940a96c75e7803feda1bc00f183ac0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003fbefda4749ba6debffbcc675dedfd16bff8d242880fa0e83fb9c460627227f23f7f3b9b9e1dc3d03f8c1c0c0e4dfd08bfc07e9dfcd0efcbc0068945fea0714c4020bbfe0d24587d000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c00c4b292d4f055c3ffebfeebc42c0bfc03235aae3f2343840234d1d2f0d8b62c019760e0f7b78b2c008c108c36ea5773fd32aef5386a1113fcd05deec1057443fda0c00f3f66e4c3fba7e5e24f6e999bfa7adc1df2b64123fb33890ad929f56000000000000000000000000");
}



} // cx
