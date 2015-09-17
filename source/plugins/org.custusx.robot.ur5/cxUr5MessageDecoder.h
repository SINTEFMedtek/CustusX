#ifndef UR5RECEIVE_H
#define UR5RECEIVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"

namespace cx
{
/**
 * Class that handles UR5 robot recieved messages.
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \author Andreas Østvik
 * \date 2015-07-10
 */


class org_custusx_robot_ur5_EXPORT Ur5MessageDecoder
{
public:
    Ur5State analyzeRawPacket(QByteArray packet);

private:
    void pushState(QByteArray data, Ur5State &state);
    QByteArray slicePacket(QByteArray data, int pos, int length);
    QByteArray getHeader(QByteArray data,int pos);
    int headerLength(QByteArray data);
    int headerID(QByteArray data);
    QByteArray removeHeader(QByteArray data);
    bool isValidPacket(QByteArray data);
    bool isValidHeader(QByteArray data);

    Ur5State setState(QByteArray data);
    Ur5State setRTState(QByteArray data);
    void setCartData(QByteArray cartData, Ur5State &state);
    void setJointData(QByteArray jointData,Ur5State &state);

    double pickDouble(QByteArray data, int pos);
    int pickInteger(QByteArray data, int pos);

    Eigen::RowVectorXd getJointPositionsRT(QByteArray data);
    Eigen::RowVectorXd getJointVelocitiesRT(QByteArray data);
    Vector3D getForceRT(QByteArray data);
    Vector3D getTorqueRT(QByteArray data);
    Vector3D getTCPAxisRT(QByteArray data);
    Vector3D getTCPAnglesRT(QByteArray data);
};


} // cx

#endif // UR5RECEIVE_H
