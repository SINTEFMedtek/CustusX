#include "cxUr5MessageDecoder.h"
#include "cxVector3D.h"
#include <set>
#include "cxUr5State.h"
#include <QDataStream>

namespace cx
{

Ur5State Ur5MessageDecoder::analyzeRawPacket(QByteArray packet)
{
    if(headerLength(packet)==560 || headerLength(packet) == 1254)
    {
        return setState(packet);
    }
    else if(headerLength(packet)==812)
    {
        return setRTState(slicePacket(packet,sizeof(int),headerLength(packet)));
    }
    else
    {
        return Ur5State(false);
    }
}

Ur5State Ur5MessageDecoder::setState(QByteArray data)
{
    Ur5State state(false);

    for(int i=0;i<data.size();i++)
    {
        if(isValidHeader(getHeader(data,i)))
        {
            pushState(slicePacket(data,i,headerLength(getHeader(data,i))),state);
        }
    }
    state.updated = true;
    return state;
}

Ur5State Ur5MessageDecoder::setRTState(QByteArray data)
{
    Ur5State state;

    sscanf_s(data.mid(0*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.timeSinceStart);

    sscanf_s(data.mid(31*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAxis(0));
    sscanf_s(data.mid(32*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAxis(1));
    sscanf_s(data.mid(33*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAxis(2));
    sscanf_s(data.mid(34*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAngles(0));
    sscanf_s(data.mid(35*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAngles(1));
    sscanf_s(data.mid(36*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAngles(2));

    sscanf_s(data.mid(37*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAxisVelocity(0));
    sscanf_s(data.mid(38*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAxisVelocity(1));
    sscanf_s(data.mid(39*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAxisVelocity(2));
    sscanf_s(data.mid(40*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAngleVelocity(0));
    sscanf_s(data.mid(41*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAngleVelocity(1));
    sscanf_s(data.mid(42*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.jointAngleVelocity(2));

    sscanf_s(data.mid(55*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.tcpAxis(0));
    sscanf_s(data.mid(56*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.tcpAxis(1));
    sscanf_s(data.mid(57*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.tcpAxis(2));
    sscanf_s(data.mid(58*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.tcpAngles(0));
    sscanf_s(data.mid(59*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.tcpAngles(1));
    sscanf_s(data.mid(60*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.tcpAngles(2));

    sscanf_s(data.mid(67*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.force(0));
    sscanf_s(data.mid(68*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.force(1));
    sscanf_s(data.mid(69*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.force(2));
    sscanf_s(data.mid(70*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.torque(0));
    sscanf_s(data.mid(71*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.torque(1));
    sscanf_s(data.mid(72*sizeof(double),sizeof(double)).toHex().data(), "%llx",(unsigned long long *)&state.torque(2));

    state.updated = true;
    return state;
}

double Ur5MessageDecoder::pickDouble(QByteArray data, int index)
{
    double a;
    QDataStream stream(data.mid(index,sizeof(double)));
    stream >> a;
    return a;
}

QByteArray Ur5MessageDecoder::getHeader(QByteArray data,int pos)
{
    return data.mid(pos,sizeof(int)+sizeof(char));
}

QByteArray Ur5MessageDecoder::slicePacket(QByteArray data,int pos, int length)
{
    return data.mid(pos,length);
}

int Ur5MessageDecoder::headerLength(QByteArray data)
{
    return pickInteger(data,0);
}

int Ur5MessageDecoder::pickInteger(QByteArray data,int pos)
{
    bool ok;
    return data.mid(pos,sizeof(int)).toHex().toInt(&ok,16);
}

int Ur5MessageDecoder::headerID(QByteArray data)
{
    bool ok;
    return data.mid((sizeof(int)),sizeof(char)).toHex().toInt(&ok,16);
}

bool Ur5MessageDecoder::isValidPacket(QByteArray data)
{
    return (data.size()==1254 || data.size()==560 || data.size()==1460);
}

bool Ur5MessageDecoder::isValidHeader(QByteArray data)
{
//    std::set<int> typeLength = {53,251,29,37,64,61}; // Cart. info, Joint data, Robot modus, Robot data1, Robot data2
//    std::set<int> types = {4,1,0,2,3,20,7}; // Cart. info, Joint data, Robot modus, Robot data1, Robot data2, Robot Messages, Force data

//    return (typeLength.find(headerLength(data)) != typeLength.end()
//            && types.find(headerID(data)) !=types.end());

    return ((headerLength(data)==53 && headerID(data)==4) || (headerLength(data)==251 && headerID(data)==1));
}

void Ur5MessageDecoder::pushState(QByteArray data,Ur5State &state)
{
    if(data.size() == 251)
    {
        setJointData(removeHeader(data),state);
    }
    if(data.size() == 53)
    {
        setCartData(removeHeader(data),state);
    }
}

QByteArray Ur5MessageDecoder::removeHeader(QByteArray data)
{
    return data.mid(sizeof(int)+sizeof(char),data.size()-sizeof(int)-sizeof(char));
}

void Ur5MessageDecoder::setCartData(QByteArray cartData,Ur5State &state)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            state.cartAxis(i) = pickDouble(cartData,i*sizeof(double));
        }
        else
        {
            state.cartAngles(i-3) = pickDouble(cartData,i*sizeof(double));
        }
    }
}

void Ur5MessageDecoder::setJointData(QByteArray jointData, Ur5State &state)
{
    for(int i=0;i<6;i++)
    {
        if(i<3)
        {
            state.jointAxis(i) = pickDouble(jointData,i*41);
            state.jointAxisVelocity(i) = pickDouble(jointData, i*41+2*sizeof(double));
        }
        else
        {
            state.jointAngles(i-3) = pickDouble(jointData,i*41);
            state.jointAngleVelocity(i-3) = pickDouble(jointData,i*41+2*sizeof(double));
        }
    }
}

} // cx
