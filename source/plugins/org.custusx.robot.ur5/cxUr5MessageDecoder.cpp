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

    state.timeSinceStart = pickDouble(data,0);

    state.jointPosition(0) = pickDouble(data,31*sizeof(double));
    state.jointPosition(1) = pickDouble(data,32*sizeof(double));
    state.jointPosition(2) = pickDouble(data,33*sizeof(double));
    state.jointPosition(3) = pickDouble(data,34*sizeof(double));
    state.jointPosition(4) = pickDouble(data,35*sizeof(double));
    state.jointPosition(5) = pickDouble(data,36*sizeof(double));

    state.jointVelocity(0) = pickDouble(data,37*sizeof(double));
    state.jointVelocity(1) = pickDouble(data,38*sizeof(double));
    state.jointVelocity(2) = pickDouble(data,39*sizeof(double));
    state.jointVelocity(3) = pickDouble(data,40*sizeof(double));
    state.jointVelocity(4) = pickDouble(data,41*sizeof(double));
    state.jointVelocity(5) = pickDouble(data,42*sizeof(double));

    state.tcpAxis(0) = pickDouble(data,55*sizeof(double));
    state.tcpAxis(1) = pickDouble(data,56*sizeof(double));
    state.tcpAxis(2) = pickDouble(data,57*sizeof(double));
    state.tcpAngles(0) = pickDouble(data,58*sizeof(double));
    state.tcpAngles(1) = pickDouble(data,59*sizeof(double));
    state.tcpAngles(2) = pickDouble(data,60*sizeof(double));

    state.force(0) = pickDouble(data,67*sizeof(double));
    state.force(1) = pickDouble(data,68*sizeof(double));
    state.force(2) = pickDouble(data,69*sizeof(double));
    state.torque(0) = pickDouble(data,70*sizeof(double));
    state.torque(1) = pickDouble(data,71*sizeof(double));
    state.torque(2) = pickDouble(data,72*sizeof(double));

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

    return (headerLength(data)==53 && headerID(data)==4);
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
        state.jointPosition(i) = pickDouble(jointData,i*41);
        state.jointVelocity(i) = pickDouble(jointData,i*41+sizeof(double));
    }
}

} // cx
