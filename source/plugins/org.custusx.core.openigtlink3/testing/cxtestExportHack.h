#ifndef EXPORTHACK_H
#define EXPORTHACK_H

#include "org_custusx_core_openigtlink3_Export.h"

//need something exported so that the lib file will be created
namespace cxtest{
class org_custusx_core_openigtlink3_EXPORT exporthack
{
public:
    exporthack(){};
    void test();
};

}//namespace cx

#endif //EXPORTHACK_H
