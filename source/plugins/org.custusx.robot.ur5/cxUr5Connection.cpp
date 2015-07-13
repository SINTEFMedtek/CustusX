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

    return mSocket->waitForBytesWritten(300);
}

bool Ur5Connection::waitForMessage()
{
    mSocket->waitForBytesWritten(200);
    return mSocket->waitForReadyRead(500);
}

void Ur5Connection::internalDataAvailable()
{
    if(!this->socketIsConnected())
        return;

    qint64 maxAvailableBytes = mSocket->bytesAvailable();

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

// Her og ned kan flyttes til Ur5Receive

void Ur5Connection::analyze_rawData()
{
    std::set<int> typeLength = {1254,560,53,251,29,37,64}; // Secondary client data x2, Cart. info, Joint data, Robot modus, Robot data1, Robot data2
    std::set<int> types = {16,4,1,0,2,3,20}; // Secondary client data, Cart. info, Joint data, Robot modus, Robot data1, Robot data2, Robot Messages

    bool ok;
    for(int i=0;i<rawData.size();i++)
    {
        int lengthOfType = rawData.mid(i,sizeof(int)).toHex().toInt(&ok,16);
        int typeID = rawData.mid((i+sizeof(int)),sizeof(char)).toHex().toInt(&ok,16);

        if(typeLength.find(lengthOfType) != typeLength.end() && types.find(typeID) !=types.end())
        {
            set_data(rawData,i,typeID,lengthOfType);
        }
    }
}

void Ur5Connection::set_data(QByteArray rawData,int i,int typeID,int lengthOfType)
{
    if(typeID==1 && lengthOfType == 251)
    {
        set_jointData(rawData.mid(i+sizeof(int)+sizeof(char),lengthOfType-sizeof(int)-sizeof(char)));
    }
    if(typeID==4)
    {
        set_cartData(rawData.mid((i+sizeof(int)+sizeof(char)),(lengthOfType-sizeof(int)-sizeof(char))));
    }
}

void Ur5Connection::set_cartData(QByteArray cartData)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            sscanf_s(cartData.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx", (unsigned long long *)&currentState.axis(i));
        }
        else
        {
            sscanf_s(cartData.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx", (unsigned long long *)&currentState.angles(i-3));
        }

    }
}

void Ur5Connection::set_jointData(QByteArray jointData)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            sscanf_s(jointData.mid(i*41+2*sizeof(double),sizeof(double)).toHex().data(), "%llx", (unsigned long long *)&jointState.axisVelocity(i));
            sscanf_s(jointData.mid(i*41,sizeof(double)).toHex().data(), "%llx", (unsigned long long *)&jointState.axis(i));
        }
        else
        {
            sscanf_s(jointData.mid(i*41+2*sizeof(double),sizeof(double)).toHex().data(), "%llx", (unsigned long long *)&jointState.angleVelocity(i-3));
            sscanf_s(jointData.mid(i*41,sizeof(double)).toHex().data(), "%llx", (unsigned long long *)&jointState.angles(i-3));
        }
    }
}


void Ur5Connection::print_cartData()
{
    std::cout << currentState.axis << std::endl;
    std::cout << currentState.angles << std::endl;
}

void Ur5Connection::print_jointData()
{
    std::cout << "Pos (x,y,z): " << jointState.axis << "  Vel (vx,vy,vz): " << jointState.axisVelocity << std::endl;
    std::cout << "Angle (wx,wy,wz):" << jointState.angles << "  Angle velocity:" << jointState.angleVelocity <<std::endl;
}

// Ur5Transmit

bool Ur5Connection::movej(Ur5State p,double a, double v,double r)
{
    targetState = p;
    QString prog = QString("movej(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8,r=%9)")
            .arg(p.axis(0)).arg(p.axis(1)).arg(p.axis(2)).arg(p.angles(0))
            .arg(p.angles(1)).arg(p.angles(2)).arg(a).arg(v).arg(r);
    if(!sendMessage(prog))
        return false;
    return true;
}

bool Ur5Connection::movel(Ur5State p,double a, double v)
{
    targetState = p;
    QString prog = QString("movel(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8)")
            .arg(p.axis(0)).arg(p.axis(1)).arg(p.axis(2)).arg(p.angles(0))
            .arg(p.angles(1)).arg(p.angles(2)).arg(a).arg(v);
    if(!sendMessage(prog))
        return false;
    return true;
}

bool Ur5Connection::speedj(double* speedField, double a, double t)
{
    QString prog = QString("speedj([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(speedField[0]).arg(speedField[1]).arg(speedField[2]).arg(speedField[3])
            .arg(speedField[4]).arg(speedField[5]).arg(a).arg(t);
    if(!sendMessage(prog))
        return false;
    return true;
}

bool Ur5Connection::waitForMove()
{
    while(!atTargetPos())
    {
        waitForMessage();
        analyze_rawData();
    }
    return true;
}

bool Ur5Connection::atTargetPos()
{
    return (currentState.axis-targetState.axis).length() < blendRadius;
}

void Ur5Connection::set_testData()
{
    rawData = QByteArray::fromHex("000004e6100000001d000000000000070b4001010100000000003ff0000000000000000000fb0140129cbfa3ff339a40129cbaa30d842e00000000000000003d8e53fd423e666741e000004253999afdbff903c903a50f28bff903da0998505c00000000000000004001d87e423e666741e66667425b999afdbff39543291650f0bff3952dca3e8fdd00000000000000003fc970ad423e666741e000004253999afdbfff1bba48ed7f84bfff1bc656ae4b4800000000000000003e832d8e423e00004204cccd425f999afd3ffa063d748b83743ffa06475f9d0b1c00000000000000003e832d8e423e6667420ccccd426c6667fdbff947eb52e92b58bff947e533bc678500000000000000003e177ab3423f999a42113333427b999afd0000003504bfc0d5af562ce9aabfdcda179aa84d0b3fe21e88537014ebc0010712885eeef94001e95d0ce34aa1bfa5d57f65e8a5cd000000e1053c6144ff1c65307f0c75ce3fa23968ff7d5bc39f3658b9ff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bfdb333333333333bfd91a9fbe76c8b40000000000000000000000000000000000000000000000003fb6d31fcd24e161000000000000000000000000000000003fbbf141205bc01a3fb83afb7e90ff973fb5119ce075f6fd3ff921fb54524550000000000000000000000000000000003ff921fb54524550bff921fb545245500000000000000000000000000000003509bfec3de20a35cbb9c02254dd7f203b37c00878db7c24eb7ebfed005a97a92bd4c002f6388e6a1839bff63172941fc304000000400300000000000000000000000000003f4401401401401400000000000000000000000000000000000041f5999a424133333f45fa0000000000080200000000250200003f927cca98db97f73f9018cae1c8068542353333003bc49ba6424c0000fd000001d506c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d18c01921fb54442d18401921fb54442d184009333333333333402e0000000000004009333333333333402e0000000000004009333333333333402e000000000000400999999999999a4039000000000000400999999999999a4039000000000000400999999999999a40390000000000003ff0c152382d73653ff657184ae744873fd00000000000003ff33333333333333fd00000000000000000000000000000bfdb333333333333bfd91a9fbe76c8b40000000000000000000000000000000000000000000000003fb6d31fcd24e161000000000000000000000000000000003fbbf141205bc01a3fb83afb7e90ff973fb5119ce075f6fd3ff921fb54524550000000000000000000000000000000003ff921fb54524550bff921fb545245500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000050000000200000001000000010000000200000002000000020000000200000002000000020000003d070000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003f752e79388a2f5900000007080001");
}


} // cx
