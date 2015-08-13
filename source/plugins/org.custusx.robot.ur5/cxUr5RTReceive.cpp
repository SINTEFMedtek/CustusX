#include "cxUr5RTReceive.h"
#include "cxVector3D.h"
#include <set>
#include "cxUr5State.h"

namespace cx
{

Ur5State Ur5RTReceive::analyze_rawPacket(QByteArray packet)
{
    if(isValidPacket(packet))
        return set_state(slicePacket(packet,4,812));
}

Ur5State Ur5RTReceive::set_state(QByteArray data)
{
    Ur5State state;

    double d[808];

    for(int i=0;i<data.size()/sizeof(double);i++)
    {
        sscanf_s(data.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&d[i]);
    }


    for(int i=0;i<data.size()/sizeof(double);i++)
    {
        std::cout << i+1 << ": " << d[i] << std::endl;
    }

    return state;
}

QByteArray Ur5RTReceive::getHeader(QByteArray data,int pos)
{
    return data.mid(pos,sizeof(int)+sizeof(char));
}

QByteArray Ur5RTReceive::slicePacket(QByteArray data,int pos, int length)
{
    return data.mid(pos,length);
}

int Ur5RTReceive::headerLength(QByteArray data)
{
    bool ok;
    return data.mid(0,sizeof(int)).toHex().toInt(&ok,16);
}

int Ur5RTReceive::headerID(QByteArray data)
{
    bool ok;
    return data.mid((sizeof(int)),sizeof(char)).toHex().toInt(&ok,16);
}

bool Ur5RTReceive::isValidPacket(QByteArray data)
{
    return (data.size()==812 || data.size()==1460);
}

void Ur5RTReceive::push_state(QByteArray data,Ur5State &state)
{
    if(data.size() == 251)
    {
        set_jointData(removeHeader(data),state);
    }
    if(data.size() == 53)
    {
        set_cartData(removeHeader(data),state);
    }
    if(data.size() == 61)
    {
        set_forceData(removeHeader(data),state);
    }
}

QByteArray Ur5RTReceive::removeHeader(QByteArray data)
{
    return data.mid(sizeof(int),data.size()-sizeof(int));
}


void Ur5RTReceive::set_cartData(QByteArray cartData,Ur5State &state)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            sscanf_s(cartData.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.cartAxis(i));
        }
        else
        {
            sscanf_s(cartData.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.cartAngles(i-3));
        }
    }
}

void Ur5RTReceive::set_jointData(QByteArray jointData, Ur5State &state)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            sscanf_s(jointData.mid(i*41+2*sizeof(double),sizeof(double)).toHex().data(), "%llx",
                     (unsigned long long *)&state.jointAxisVelocity(i));
            sscanf_s(jointData.mid(i*41,sizeof(double)).toHex().data(), "%llx",
                     (unsigned long long *)&state.jointAxis(i));
        }
        else
        {
            sscanf_s(jointData.mid(i*41+2*sizeof(double),sizeof(double)).toHex().data(), "%llx",
                     (unsigned long long *)&state.jointAngleVelocity(i-3));
            sscanf_s(jointData.mid(i*41,sizeof(double)).toHex().data(), "%llx",
                     (unsigned long long *)&state.jointAngles(i-3));
        }
    }
}

void Ur5RTReceive::set_forceData(QByteArray forceData, Ur5State &state)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            sscanf_s(forceData.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.force(i));
        }
        else
        {
            sscanf_s(forceData.mid(i*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.torque(i-3));
        }
    }
}


void Ur5RTReceive::print_cartData(Ur5State state)
{
    std::cout << state.cartAxis << std::endl;
    std::cout << state.cartAngles << std::endl;
}

void Ur5RTReceive::print_jointData(Ur5State state)
{
    std::cout << "Pos (x,y,z): " << state.jointAxis << " " <<
                 "Vel (vx,vy,vz): " << state.jointAxisVelocity << std::endl;
    std::cout << "Angle (wx,wy,wz):" << state.jointAngles << " " <<
                 "Angle velocity:" << state.jointAngleVelocity <<std::endl;
}


void Ur5RTReceive::print_rawData(QByteArray data)
{
    std::cout << "Number of bytes: " << data.size() << std::endl;
    for(int i=0;i<data.size();i++)
    {
        std::cout << data.data()[i];
    }
    std::cout << std::endl;
}

} // cx
