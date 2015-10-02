#include "cxProtocol.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"

namespace cx
{

Protocol::Protocol(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");
    qRegisterMetaType<ProbeDefinitionPtr>("ProbeDefinitionPtr");
}

QString Protocol::getName() const
{
    return "Basic";
}

EncodedPackagePtr Protocol::getPack()
{
    return mPack;
}



} //namespace cx
