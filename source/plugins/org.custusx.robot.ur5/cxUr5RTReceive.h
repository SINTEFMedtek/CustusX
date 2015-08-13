#ifndef UR5RTRECEIVE_H
#define UR5RTRECEIVE_H

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

class org_custusx_robot_ur5_EXPORT Ur5RTReceive
{
public:
    Ur5State analyze_rawPacket(QByteArray packet);

    Ur5State set_state(QByteArray data);

    QByteArray getHeader(QByteArray data,int pos);
    int headerLength(QByteArray data);
    int headerID(QByteArray data);
    QByteArray removeHeader(QByteArray data);

    bool isValidPacket(QByteArray data);

    QByteArray slicePacket(QByteArray data, int pos, int length);

    void push_state(QByteArray data, Ur5State &state);
    void set_cartData(QByteArray cartData, Ur5State &state);
    void set_jointData(QByteArray jointData,Ur5State &state);
    void set_forceData(QByteArray forceData,Ur5State &state);

    void print_cartData(Ur5State);
    void print_cartData(Ur5StatePtr);
    void print_jointData(Ur5State);
    void print_rawData(QByteArray data);

    QByteArray removeFirstNBytes(QByteArray data,int n);
};


} // cx

#endif // UR5RTRECEIVE_H
