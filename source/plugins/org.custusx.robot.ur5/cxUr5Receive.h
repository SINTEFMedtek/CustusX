#ifndef UR5RECEIVE_H
#define UR5RECEIVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5State.h"

namespace cx
{
class org_custusx_robot_ur5_EXPORT Ur5Receive
{
public:
    Ur5State analyze_rawPacket(QByteArray packet);

    Ur5State set_state(QByteArray data);

    QByteArray getHeader(QByteArray data,int pos);
    int headerLength(QByteArray data);
    int headerID(QByteArray data);
    QByteArray removeHeader(QByteArray data);

    bool isValidPacket(QByteArray data);
    bool isValidHeader(QByteArray data);

    QByteArray slicePacket(QByteArray data, int pos, int length);


    void push_state(QByteArray data, Ur5State &state);
    void set_cartData(QByteArray cartData, Ur5State &state);
    void set_jointData(QByteArray jointData,Ur5State &state);

    void print_cartData(Ur5State);
    void print_jointData(Ur5State);

    QByteArray removeFirstNBytes(QByteArray data,int n);
};


} // cx

#endif // UR5RECEIVE_H
