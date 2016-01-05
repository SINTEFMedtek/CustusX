#include "cxUr5Kinematics.h"

namespace cx
{

Eigen::MatrixXd Ur5Kinematics::forward(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::RowVectorXd s(6), c(6);

    s << sin(jointConfiguration(0)), sin(jointConfiguration(1)), sin(jointConfiguration(2)),
            sin(jointConfiguration(3)), sin(jointConfiguration(4)), sin(jointConfiguration(5));

    c << cos(jointConfiguration(0)), cos(jointConfiguration(1)), cos(jointConfiguration(2)),
            cos(jointConfiguration(3)), cos(jointConfiguration(4)), cos(jointConfiguration(5));


    Eigen::MatrixXd matrix(4,4);

    matrix << c(5)*(s(0)*s(4) - c(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))) - s(5)*(c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2))),
            - s(5)*(s(0)*s(4) - c(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))) - c(5)*(c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2))),
            c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))),
            (2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400,
            - c(5)*(c(0)*s(4) + c(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))) - s(5)*(c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0))),
            s(5)*(c(0)*s(4) + c(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))) - c(5)*(c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0))),
            s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))) - c(0)*c(4),
            (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 - (823*c(0)*c(4))/10000 - (17*c(1)*s(0))/40 - (2183*c(0))/20000 - (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 + (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 + (157*s(0)*s(1)*s(2))/400 - (157*c(1)*c(2)*s(0))/400,
            s(5)*(c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1))) + c(4)*c(5)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))),
            c(5)*(c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1))) - c(4)*s(5)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))),
            -s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))),
            (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000 - (157*c(1)*s(2))/400 - (157*c(2)*s(1))/400 - (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 - (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (17*s(1))/40 + 2229/25000,
            0, 0, 0, 1;

    return matrix;
}

Eigen::MatrixXd Ur5Kinematics::forward2(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::MatrixXd matrix(4,4);

    double s1 = sin(jointConfiguration(0)), c1 = cos(jointConfiguration(0));
    double q234 = jointConfiguration(1), s2 = sin(jointConfiguration(1)), c2 = cos(jointConfiguration(1));
    double s3 = sin(jointConfiguration(2)), c3 = cos(jointConfiguration(2)); q234 += jointConfiguration(2);
    q234 += jointConfiguration(3);
    double s5 = sin(jointConfiguration(4)), c5 = cos(jointConfiguration(4));
    double s6 = sin(jointConfiguration(5)), c6 = cos(jointConfiguration(5));
    double s234 = sin(q234), c234 = cos(q234);

    matrix << ((c1*c234-s1*s234)*s5)/2.0 - c5*s1 + ((c1*c234+s1*s234)*s5)/2.0,
            (c6*(s1*s5 + ((c1*c234-s1*s234)*c5)/2.0 + ((c1*c234+s1*s234)*c5)/2.0) - (s6*((s1*c234+c1*s234) - (s1*c234-c1*s234)))/2.0),
            (-(c6*((s1*c234+c1*s234) - (s1*c234-c1*s234)))/2.0 - s6*(s1*s5 + ((c1*c234-s1*s234)*c5)/2.0 + ((c1*c234+s1*s234)*c5)/2.0)),
            ((d5*(s1*c234-c1*s234))/2.0 - (d5*(s1*c234+c1*s234))/2.0 - d4*s1 + (d6*(c1*c234-s1*s234)*s5)/2.0 + (d6*(c1*c234+s1*s234)*s5)/2.0 - a2*c1*c2 - d6*c5*s1 - a3*c1*c2*c3 + a3*c1*s2*s3),
            c1*c5 + ((s1*c234+c1*s234)*s5)/2.0 + ((s1*c234-c1*s234)*s5)/2.0,
            (c6*(((s1*c234+c1*s234)*c5)/2.0 - c1*s5 + ((s1*c234-c1*s234)*c5)/2.0) + s6*((c1*c234-s1*s234)/2.0 - (c1*c234+s1*s234)/2.0)),
            (c6*((c1*c234-s1*s234)/2.0 - (c1*c234+s1*s234)/2.0) - s6*(((s1*c234+c1*s234)*c5)/2.0 - c1*s5 + ((s1*c234-c1*s234)*c5)/2.0)),
            ((d5*(c1*c234-s1*s234))/2.0 - (d5*(c1*c234+s1*s234))/2.0 + d4*c1 + (d6*(s1*c234+c1*s234)*s5)/2.0 + (d6*(s1*c234-c1*s234)*s5)/2.0 + d6*c1*c5 - a2*c2*s1 - a3*c2*c3*s1 + a3*s1*s2*s3),
            ((c234*c5-s234*s5)/2.0 - (c234*c5+s234*s5)/2.0),
            ((s234*c6-c234*s6)/2.0 - (s234*c6+c234*s6)/2.0 - s234*c5*c6),
            (s234*c5*s6 - (c234*c6+s234*s6)/2.0 - (c234*c6-s234*s6)/2.0),
            (d1 + (d6*(c234*c5-s234*s5))/2.0 + a3*(s2*c3+c2*s3) + a2*s2 - (d6*(c234*c5+s234*s5))/2.0 - d5*c234),
            0.0, 0.0, 0.0, 1.0;

    return matrix;
}


Eigen::MatrixXd Ur5Kinematics::jacobian(Eigen::RowVectorXd jointConfiguration)
{
    Eigen::RowVectorXd s(6), c(6);

    s << sin(jointConfiguration(0)), sin(jointConfiguration(1)), sin(jointConfiguration(2)),
            sin(jointConfiguration(3)), sin(jointConfiguration(4)), sin(jointConfiguration(5));

    c << cos(jointConfiguration(0)), cos(jointConfiguration(1)), cos(jointConfiguration(2)),
            cos(jointConfiguration(3)), cos(jointConfiguration(4)), cos(jointConfiguration(5));


    Eigen::MatrixXd matrix(6,6);

    matrix <<   (2183*c(0))/20000 + (823*c(0)*c(4))/10000 + (17*c(1)*s(0))/40 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400,
                c(0)*((17*s(1))/40 + (157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                c(0)*((157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                c(0)*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                -(c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1)))*((823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000) - (c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*(c(0)*c(4) - s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))))/10000 - s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*((823*c(0)*c(4))/10000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                (2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400,
                s(0)*((17*s(1))/40 + (157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                s(0)*((157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                s(0)*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
                (c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000) - (c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1)))*((823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000),
                (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*(c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))))/10000 - s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*((823*c(4)*s(0))/10000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000),
                0,
                c(0)*((2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 + (17*c(1)*s(0))/40 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400),
                c(0)*((2183*s(0))/20000 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400),
                c(0)*((2183*s(0))/20000 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                -(c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))*((823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000) - (c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))*((823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                ((823*c(4)*s(0))/10000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000)*(c(0)*c(4) - s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))) - (c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))*((823*c(0)*c(4))/10000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
                0,
                s(0),
                s(0),
                s(0),
                c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)),
                c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))),
                0,
                -c(0),
                -c(0),
                -c(0),
                c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)),
                s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))) - c(0)*c(4),
                1,
                0,
                0,
                0,
                s(3)*(c(1)*s(2) + c(2)*s(1)) - c(3)*(c(1)*c(2) - s(1)*s(2)),
                -s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)));

    return matrix;
}

Eigen::MatrixXd Ur5Kinematics::inverse(Eigen::MatrixXd T)
{
    Eigen::MatrixXd q_sols(8,6);
    int num_sols = 0;
    double q6_des = 0;

    double T00 =  T(0,0); double T01 = T(0,1); double T02 = T(0,2);  double T03 = T(0,3);
    double T12 =  T(1,2); double T10 = T(1,0); double T11 = T(1,1);  double T13 = T(1,3);
    double T22 =  T(2,2); double T20 = T(2,0); double T21 = T(2,1);  double T23 = T(2,3);

    ////////////////////////////// shoulder rotate joint (q1) //////////////////////////////
    double q1[2];
    {
        double A = d6*T12 - T13;
        double B = d6*T02 - T03;
        double R = A*A + B*B;
        if(fabs(A) < ZERO_THRESH) {
            double div;
            if(fabs(fabs(d4) - fabs(B)) < ZERO_THRESH)
                div = -SIGN(d4)*SIGN(B);
            else
                div = -d4/B;
            double arcsin = asin(div);
            if(fabs(arcsin) < ZERO_THRESH)
                arcsin = 0.0;
            if(arcsin < 0.0)
                q1[0] = arcsin;
            else
                q1[0] = arcsin;
            q1[1] = - arcsin;
        }
        else if(fabs(B) < ZERO_THRESH) {
            double div;
            if(fabs(fabs(d4) - fabs(A)) < ZERO_THRESH)
                div = SIGN(d4)*SIGN(A);
            else
                div = d4/A;
            double arccos = acos(div);
            q1[0] = arccos;
            q1[1] = - arccos;
        }
        else if(d4*d4 > R) {
            Eigen::MatrixXd emptyMatrix(1,6);
            return emptyMatrix;
        }
        else {
            double arccos = acos(d4 / sqrt(R)) ;
            double arctan = atan2(-B, A);
            double pos = arccos + arctan;
            double neg = -arccos + arctan;
            if(fabs(pos) < ZERO_THRESH)
                pos = 0.0;
            if(fabs(neg) < ZERO_THRESH)
                neg = 0.0;
            if(pos >= 0.0)
                q1[0] = pos;
            else
                q1[0] = pos;
            if(neg >= 0.0)
                q1[1] = neg;
            else
                q1[1] = neg;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////// wrist 2 joint (q5) //////////////////////////////
    double q5[2][2];
    {
        for(int i=0;i<2;i++) {
            double numer = (T03*sin(q1[i]) - T13*cos(q1[i])-d4);
            double div;
            if(fabs(fabs(numer) - fabs(d6)) < ZERO_THRESH)
                div = SIGN(numer) * SIGN(d6);
            else
                div = numer / d6;
            double arccos = acos(div);
            q5[i][0] = arccos;
            q5[i][1] = - arccos;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    {
        for(int i=0;i<2;i++) {
            for(int j=0;j<2;j++) {
                double c1 = cos(q1[i]), s1 = sin(q1[i]);
                double c5 = cos(q5[i][j]), s5 = sin(q5[i][j]);
                double q6;
                ////////////////////////////// wrist 3 joint (q6) //////////////////////////////
                if(fabs(s5) < ZERO_THRESH)
                    q6 = q6_des;
                else {
                    q6 = atan2(SIGN(s5)*-(T01*s1 - T11*c1),
                               SIGN(s5)*(T00*s1 - T10*c1));
                    if(fabs(q6) < ZERO_THRESH)
                        q6 = 0.0;
                    if(q6 < 0.0)
                        q6 += 0;
                }
                ////////////////////////////////////////////////////////////////////////////////

                double q2[2], q3[2], q4[2];
                ///////////////////////////// RRR joints (q2,q3,q4) ////////////////////////////
                double c6 = cos(q6), s6 = sin(q6);
                double x04x = -s5*(T02*c1 + T12*s1) - c5*(s6*(T01*c1 + T11*s1) - c6*(T00*c1 + T10*s1));
                double x04y = c5*(T20*c6 - T21*s6) - T22*s5;
                double p13x = d5*(s6*(T00*c1 + T10*s1) + c6*(T01*c1 + T11*s1)) - d6*(T02*c1 + T12*s1) +
                        T03*c1 + T13*s1;
                double p13y = T23 - d1 - d6*T22 + d5*(T21*c6 + T20*s6);

                double c3 = (p13x*p13x + p13y*p13y - a2*a2 - a3*a3) / (2.0*a2*a3);
                if(fabs(fabs(c3) - 1.0) < ZERO_THRESH)
                    c3 = SIGN(c3);
                else if(fabs(c3) > 1.0) {
                    // TODO NO SOLUTION
                    continue;
                }
                double arccos = acos(c3);
                q3[0] = arccos;
                q3[1] = - arccos;
                double denom = a2*a2 + a3*a3 + 2*a2*a3*c3;
                double s3 = sin(arccos);
                double A = (a2 + a3*c3), B = a3*s3;
                q2[0] = atan2((A*p13y - B*p13x) / denom, (A*p13x + B*p13y) / denom);
                q2[1] = atan2((A*p13y + B*p13x) / denom, (A*p13x - B*p13y) / denom);
                double c23_0 = cos(q2[0]+q3[0]);
                double s23_0 = sin(q2[0]+q3[0]);
                double c23_1 = cos(q2[1]+q3[1]);
                double s23_1 = sin(q2[1]+q3[1]);
                q4[0] = atan2(c23_0*x04y - s23_0*x04x, x04x*c23_0 + x04y*s23_0);
                q4[1] = atan2(c23_1*x04y - s23_1*x04x, x04x*c23_1 + x04y*s23_1);
                ////////////////////////////////////////////////////////////////////////////////

                for(int k=0;k<2;k++) {
                    if(fabs(q2[k]) < ZERO_THRESH)
                        q2[k] = 0.0;
                    else if(q2[k] < 0.0) q2[k] += 0;
                    if(fabs(q4[k]) < ZERO_THRESH)
                        q4[k] = 0.0;
                    else if(q4[k] < 0.0) q4[k] += 0;
                    q_sols(num_sols,0) = q1[i];
                    q_sols(num_sols,1) = q2[k];
                    q_sols(num_sols,2) = q3[k];
                    q_sols(num_sols,3) = q4[k];
                    q_sols(num_sols,4) = q5[i][j];
                    q_sols(num_sols,5) = q6;

                    //q_sols[num_sols*6+0] = q1[i];    q_sols[num_sols*6+1] = q2[k];
                    //q_sols[num_sols*6+2] = q3[k];    q_sols[num_sols*6+3] = q4[k];
                    //q_sols[num_sols*6+4] = q5[i][j]; q_sols[num_sols*6+5] = q6;
                    num_sols++;
                }

            }
        }
    }
    return q_sols;
}

Transform3D Ur5Kinematics::poseToMatrix(Eigen::RowVectorXd poseConfiguration)
{
    Transform3D matrix;

    Vector3D cartAngles;
    cartAngles << poseConfiguration(3), poseConfiguration(4), poseConfiguration(5);

    Vector3D cartAxis;
    cartAxis << poseConfiguration(0), poseConfiguration(1), poseConfiguration(2);

    matrix = Eigen::AngleAxisd(cartAngles.norm(),cartAngles/cartAngles.norm());
    matrix.translation() = cartAxis*1000;

    return matrix;
}

Vector3D Ur5Kinematics::T2transl(Eigen::MatrixXd T)
{
    Vector3D points;

    points << -T(0,3), -T(1,3), T(2,3);
    return points;
}

Vector3D Ur5Kinematics::T2transl(Transform3D T)
{
    Vector3D points;

    points << -T(0,3), -T(1,3), T(2,3);
    return points;
}

Eigen::RowVectorXd Ur5Kinematics::inverseJ(Eigen::MatrixXd desiredPose, Eigen::RowVectorXd guessedJointConfiguration)
{
    double errorThreshold = 10^-6;
    double K = 1;

    Eigen::RowVectorXd jointConfiguration(6),error(6),dq(6);
    jointConfiguration = guessedJointConfiguration;

    Eigen::MatrixXd jacobi(6,6), currentPose(4,4);

    currentPose = forward2(jointConfiguration);
    error = errorVector(desiredPose,currentPose);

    while(error.length()>errorThreshold)
    {
        jacobi = jacobian(jointConfiguration);
        currentPose = forward2(jointConfiguration);

        dq = K*pseudoInverse(jacobi)*errorVector(desiredPose,currentPose);
        jointConfiguration = jointConfiguration + dq;

        error = errorVector(desiredPose,currentPose);
    }

    return jointConfiguration;
}

Eigen::RowVectorXd Ur5Kinematics::errorVector(Eigen::MatrixXd desiredPose, Eigen::MatrixXd currentPose)
{
    Eigen::RowVectorXd e(6);

    e << desiredPose(0,3)-currentPose(0,3),desiredPose(1,3)-currentPose(1,3), desiredPose(2,3)-currentPose(2,3);

    Eigen::MatrixXd Rqd, Rqe, R;
    Rqd = getRotation(desiredPose);
    Rqe = getRotation(currentPose);

    R = Rqd*Rqe.transpose();

    e << 0.5*(R(2,1)-R(1,2)), 0.5*(R(0,2)-R(2,0)), 0.5*(R(1,0)-R(0,1));

    return e;
}

Eigen::MatrixXd Ur5Kinematics::getRotation(Eigen::MatrixXd pose)
{
    return pose.block(0,0,3,3);
}

Eigen::MatrixXd Ur5Kinematics::pseudoInverse(Eigen::MatrixXd matrix)
{
    return matrix.transpose()*(matrix*matrix.transpose()).inverse();
}

} // cx
