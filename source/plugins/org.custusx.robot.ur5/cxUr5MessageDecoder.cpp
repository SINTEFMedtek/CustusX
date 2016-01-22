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
    setTransformationMatrix(state);
    state.updated = true;
    return state;
}

Ur5State Ur5MessageDecoder::setRTState(QByteArray data)
{
    Ur5State state;

    state.timeSinceStart = pickDouble(data,0);
    state.jointConfiguration = getJointPositionsRT(data);
    state.jointVelocity = getJointVelocitiesRT(data);
    //state.tcpAxis = getTCPAxisRT(data);
    //state.tcpAngles = getTCPAnglesRT(data);
    //state.force = getForceRT(data);
    //state.torque = getTorqueRT(data);
    state.updated = true;

    return state;
}

Eigen::RowVectorXd Ur5MessageDecoder::getJointPositionsRT(QByteArray data)
{
    Eigen::RowVectorXd jp(6);
    for(int i=0;i<6;i++)
    {
        jp(i) = pickDouble(data,(31+i)*sizeof(double));
    }
    return jp;
}

Eigen::RowVectorXd Ur5MessageDecoder::getJointVelocitiesRT(QByteArray data)
{
    Eigen::RowVectorXd jv(6);
    for(int i=0;i<6;i++)
    {
        jv(i) = pickDouble(data,(37+i)*sizeof(double));
    }
    return jv;
}

Vector3D Ur5MessageDecoder::getTCPAxisRT(QByteArray data)
{
    Vector3D tcpAxis;
    for(int i=0;i<3;i++)
    {
        tcpAxis(i) = pickDouble(data,(55+i)*sizeof(double));
    }
    return tcpAxis;
}

Vector3D Ur5MessageDecoder::getTCPAnglesRT(QByteArray data)
{
    Vector3D tcpAngles;
    for(int i=0;i<3;i++)
    {
        tcpAngles(i) = pickDouble(data,(58+i)*sizeof(double));
    }
    return tcpAngles;
}

Vector3D Ur5MessageDecoder::getForceRT(QByteArray data)
{
    Vector3D force;
    for(int i=0;i<3;i++)
    {
        force(i) = pickDouble(data,(67+i)*sizeof(double));
    }
    return force;
}

Vector3D Ur5MessageDecoder::getTorqueRT(QByteArray data)
{
    Vector3D torque;
    for(int i=0;i<3;i++)
    {
        torque(i) = pickDouble(data,(70+i)*sizeof(double));
    }
    return torque;
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

void Ur5MessageDecoder::setTransformationMatrix(Ur5State &state)
{
    state.baseMee = Eigen::AngleAxisd(state.cartAngles.norm(),state.cartAngles/state.cartAngles.norm());
    state.baseMee.translation() = state.cartAxis*1000;
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
        state.jointConfiguration(i) = pickDouble(jointData,i*41);
        state.jointVelocity(i) = pickDouble(jointData,i*41+sizeof(double));
    }
}

} // cx
