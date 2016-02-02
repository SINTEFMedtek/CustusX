#include "cxUr5MessageEncoder.h"
#include "cxUr5State.h"


namespace cx
{

QString Ur5MessageEncoder::movej(Ur5State p,double a, double v,double r)
{
    return QString("movej(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8,r=%9)")
            .arg(p.cartAxis(0)/1000).arg(p.cartAxis(1)/1000).arg(p.cartAxis(2)/1000)
            .arg(p.cartAngles(0)).arg(p.cartAngles(1)).arg(p.cartAngles(2)).arg(a).arg(v).arg(r);
}

QString Ur5MessageEncoder::movej(Eigen::RowVectorXd p,double a, double v,double t,double r)
{
    return QString("movej([%1,%2,%3,%4,%5,%6],a=%7,v=%8,r=%9)")
            .arg(p(0)).arg(p(1)).arg(p(2)).arg(p(3)).arg(p(4)).arg(p(5)).arg(a).arg(v).arg(t).arg(r);
}

QString Ur5MessageEncoder::movej(Eigen::RowVectorXd p,double t)
{
    return QString("movej([%1,%2,%3,%4,%5,%6],a=1.4,v=1.05,t=%7)")
            .arg(p(0)).arg(p(1)).arg(p(2)).arg(p(3)).arg(p(4)).arg(p(5)).arg(t);
}

QString Ur5MessageEncoder::movejp(Eigen::RowVectorXd p,double a, double v,double t,double r)
{
    return QString("movej(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8,r=%9)")
            .arg(p(0)/1000).arg(p(1)/1000).arg(p(2)/1000)
            .arg(p(3)).arg(p(4)).arg(p(5)).arg(a).arg(v).arg(r);
}

QString Ur5MessageEncoder::movej(Ur5MovementInfo m)
{
    if(m.time != 0 && m.spaceFlag == Ur5MovementInfo::jointSpace)
    {
        return QString("movej([%1,%2,%3,%4,%5,%6],a=1.4,v=1.05,t=%7)")
                .arg(m.targetJointConfiguration(0)).arg(m.targetJointConfiguration(1))
                .arg(m.targetJointConfiguration(2)).arg(m.targetJointConfiguration(3))
                .arg(m.targetJointConfiguration(4)).arg(m.targetJointConfiguration(5))
                .arg(m.time);
    }
    else if(m.time > 0 && m.spaceFlag == Ur5MovementInfo::jointSpace)
    {
        return QString("movej([%1,%2,%3,%4,%5,%6],a=%7,v=%8,t=%9,r=%10)")
                .arg(m.targetJointConfiguration(0)).arg(m.targetJointConfiguration(1))
                .arg(m.targetJointConfiguration(2)).arg(m.targetJointConfiguration(3))
                .arg(m.targetJointConfiguration(4)).arg(m.targetJointConfiguration(5))
                .arg(m.acceleration).arg(m.velocity).arg(m.time).arg(m.radius);
    }
    if(m.time != 0 && m.spaceFlag == Ur5MovementInfo::operationalSpace)
    {
        return QString("movej(p[%1,%2,%3,%4,%5,%6],a=1.4,v=1.05,t=%7)")
                .arg(m.targetJointConfiguration(0)).arg(m.targetJointConfiguration(1))
                .arg(m.targetJointConfiguration(2)).arg(m.targetJointConfiguration(3))
                .arg(m.targetJointConfiguration(4)).arg(m.targetJointConfiguration(5))
                .arg(m.time);
    }
    else if(m.time > 0 && m.spaceFlag == Ur5MovementInfo::operationalSpace)
    {
        return QString("movej(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8,t=%9,r=%10)")
                .arg(m.targetJointConfiguration(0)).arg(m.targetJointConfiguration(1))
                .arg(m.targetJointConfiguration(2)).arg(m.targetJointConfiguration(3))
                .arg(m.targetJointConfiguration(4)).arg(m.targetJointConfiguration(5))
                .arg(m.acceleration).arg(m.velocity).arg(m.time).arg(m.radius);
    }
    else
    {
        return QString("invalid");
    }
}

QString Ur5MessageEncoder::movel(Ur5State p,double a, double v)
{
    return QString("movel(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8)")
            .arg(p.cartAxis(0)/1000).arg(p.cartAxis(1)/1000).arg(p.cartAxis(2)/1000)
            .arg(p.cartAngles(0)).arg(p.cartAngles(1)).arg(p.cartAngles(2)).arg(a).arg(v);
}

QString Ur5MessageEncoder::movec(Ur5State pose_via,Ur5State pose_to, double a, double v, double r)
{
    return QString("movec(p[%1,%2,%3,%4,%5,%6],p[%7,%8,%9,%10,%11,%12],a=%13,v=%14,r=%15")
            .arg(pose_via.cartAxis(0)/1000).arg(pose_via.cartAxis(1)/1000).arg(pose_via.cartAxis(2)/1000)
            .arg(pose_via.cartAngles(0)).arg(pose_via.cartAngles(1)).arg(pose_via.cartAngles(2))
            .arg(pose_to.cartAxis(0)/1000).arg(pose_to.cartAxis(1)/1000).arg(pose_to.cartAxis(2)/1000)
            .arg(pose_to.cartAngles(0)).arg(pose_to.cartAngles(1)).arg(pose_to.cartAngles(2))
            .arg(a).arg(v).arg(r);
}

QString Ur5MessageEncoder::speedj(Ur5State p, double a, double t)
{
    return QString("speedj([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(p.jointVelocity(0)).arg(p.jointVelocity(1)).arg(p.jointVelocity(2)).arg(p.jointVelocity(3))
            .arg(p.jointVelocity(4)).arg(p.jointVelocity(5)).arg(a).arg(t);
}

QString Ur5MessageEncoder::speedj(Eigen::RowVectorXd jointVelocity, double a, double t)
{
    return QString("speedj([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(jointVelocity(0)).arg(jointVelocity(1)).arg(jointVelocity(2)).arg(jointVelocity(3))
            .arg(jointVelocity(4)).arg(jointVelocity(5)).arg(a).arg(t);
}

QString Ur5MessageEncoder::speedj(Ur5MovementInfo m)
{
    return QString("speedj([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(m.targetJointVelocity(0)).arg(m.targetJointVelocity(1))
            .arg(m.targetJointVelocity(2)).arg(m.targetJointVelocity(3))
            .arg(m.targetJointVelocity(4)).arg(m.targetJointVelocity(5))
            .arg(m.acceleration).arg(m.time);
}

QString Ur5MessageEncoder::speedl(Ur5State p, double a, double t)
{
    return QString("speedl([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(p.operationalVelocity(0)).arg(p.operationalVelocity(1)).arg(p.operationalVelocity(2))
            .arg(p.operationalVelocity(3)).arg(p.operationalVelocity(4)).arg(p.operationalVelocity(5)).arg(a).arg(t);
}

QString Ur5MessageEncoder::speedl(Eigen::RowVectorXd linearVelocity, double a, double t)
{
    return QString("speedl([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(linearVelocity(0)).arg(linearVelocity(1)).arg(linearVelocity(2)).arg(linearVelocity(3))
            .arg(linearVelocity(4)).arg(linearVelocity(5)).arg(a).arg(t);
}

QString Ur5MessageEncoder::set_tcp(Ur5State p)
{
    return QString("set_tcp(p[%1,%2,%3,%4,%5,%6])")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2));
}

QString Ur5MessageEncoder::stopl(double a)
{
    return QString("stopl(%1)").arg(a);
}

QString Ur5MessageEncoder::stopj(double a)
{
    return QString("stopj(%1)").arg(a);
}

QString Ur5MessageEncoder::powerdown()
{
    return QString("powerdown()");
}

QString Ur5MessageEncoder::textmsg(QString str)
{
    return QString("textmsg(%1)").arg(str);
}

QString Ur5MessageEncoder::sleep(double t)
{
    return QString("sleep(%1)").arg(t);
}



} // cx

