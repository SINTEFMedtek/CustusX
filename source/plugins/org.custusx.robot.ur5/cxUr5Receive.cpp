#include "cxUr5Receive.h"
#include "cxVector3D.h"
#include <set>
#include "cxUr5State.h"

namespace cx
{

Ur5State Ur5Receive::analyze_rawPacket(QByteArray packet)
{
    if(isValidPacket(packet))
        return set_state(packet);
}

Ur5State Ur5Receive::set_state(QByteArray data)
{
    Ur5State state;
    for(int i=0;i<data.size();i++)
    {
        if(isValidHeader(getHeader(data,i)))
        {
            push_state(slicePacket(data,i,headerLength(getHeader(data,i))),state);
        }
    }
    return state;
}

QByteArray Ur5Receive::getHeader(QByteArray data,int pos)
{
    return data.mid(pos,sizeof(int)+sizeof(char));
}

QByteArray Ur5Receive::slicePacket(QByteArray data,int pos, int length)
{
    return data.mid(pos,length);
}

int Ur5Receive::headerLength(QByteArray data)
{
    bool ok;
    return data.mid(0,sizeof(int)).toHex().toInt(&ok,16);
}

int Ur5Receive::headerID(QByteArray data)
{
    bool ok;
    return data.mid((sizeof(int)),sizeof(char)).toHex().toInt(&ok,16);
}

bool Ur5Receive::isValidPacket(QByteArray data)
{
    return (data.size()==1254 || data.size()==560);
}

bool Ur5Receive::isValidHeader(QByteArray data)
{
    std::set<int> typeLength = {53,251,29,37,64,61}; // Cart. info, Joint data, Robot modus, Robot data1, Robot data2
    std::set<int> types = {4,1,0,2,3,20,7}; // Cart. info, Joint data, Robot modus, Robot data1, Robot data2, Robot Messages, Force data

    //std::cout << headerID(data) << " " << headerLength(data) << std::endl;
    return (typeLength.find(headerLength(data)) != typeLength.end()
            && types.find(headerID(data)) !=types.end());
}

void Ur5Receive::push_state(QByteArray data,Ur5State &state)
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

QByteArray Ur5Receive::removeHeader(QByteArray data)
{
    return data.mid(sizeof(int)+sizeof(char),data.size()-sizeof(int)-sizeof(char));
}


void Ur5Receive::set_cartData(QByteArray cartData,Ur5State &state)
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

void Ur5Receive::set_jointData(QByteArray jointData, Ur5State &state)
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

void Ur5Receive::set_forceData(QByteArray forceData, Ur5State &state)
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


void Ur5Receive::print_cartData(Ur5State state)
{
    std::cout << state.cartAxis << std::endl;
    std::cout << state.cartAngles << std::endl;
}

void Ur5Receive::print_jointData(Ur5State state)
{
    std::cout << "Pos (x,y,z): " << state.jointAxis << " " <<
                 "Vel (vx,vy,vz): " << state.jointAxisVelocity << std::endl;
    std::cout << "Angle (wx,wy,wz):" << state.jointAngles << " " <<
                 "Angle velocity:" << state.jointAngleVelocity <<std::endl;
}

} // cx
