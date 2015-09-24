#include "cxProtocol.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionBase.h"
#include "cxIGTLinkConversionPolyData.h"

namespace cx
{

Protocol::Protocol(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");
    qRegisterMetaType<ProbeDefinitionPtr>("ProbeDefinitionPtr");
    qRegisterMetaType<IGTLinkUSStatusMessage::Pointer>("IGTLinkUSStatusMessage::Pointer");
    qRegisterMetaType<IGTLinkImageMessage::Pointer>("IGTLinkImageMessage::Pointer");
}

QString Protocol::getName() const
{
    return "Basic";
}

bool Protocol::doCRC() const
{
    return true;
}

void Protocol::translate(const igtl::TransformMessage::Pointer body)
{
    QString deviceName = body->GetDeviceName();

    IGTLinkConversion converter;
    Transform3D prMs = converter.decode(body);

	IGTLinkConversionBase baseConverter;
	double timestamp_ms = baseConverter.decode_timestamp(body).toMSecsSinceEpoch();

    emit transform(deviceName, prMs, timestamp_ms);
}

void Protocol::translate(const igtl::PolyDataMessage::Pointer body)
{
	this->writeAcceptingMessage(body);

	IGTLinkConversionPolyData polyConverter;
	MeshPtr retval = polyConverter.decode(body, this->coordinateSystem());
	emit mesh(retval);
}

void Protocol::translate(const igtl::ImageMessage::Pointer body)
{
	this->writeAcceptingMessage(body);

	IGTLinkConversionImage imageConverter;
	ImagePtr retval = imageConverter.decode(body);
	emit image(retval);
}

void Protocol::translate(const igtl::StatusMessage::Pointer body)
{
	IGTLinkConversion converter;
    QString status = converter.decode(body);
	CX_LOG_CHANNEL_VOLATILE(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL status: " << status;
}

void Protocol::translate(igtl::StringMessage::Pointer body)
{
	IGTLinkConversion converter;
    QString string = converter.decode(body);
    //This was spamming the console
	CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL string: " << string;
}

void Protocol::writeNotSupportedMessage(igtl::MessageBase* body)
{
	QString dtype(body->GetDeviceType());
	QString dname(body->GetDeviceName());
	CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << QString("Ignoring incoming igtlink message (%1, %2): ")
														.arg(dtype)
														.arg(dname);
}

void Protocol::writeAcceptingMessage(igtl::MessageBase* body)
{
	QString dtype(body->GetDeviceType());
	QString dname(body->GetDeviceName());
	CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << QString("Accepting incoming igtlink message (%1, %2): ")
														.arg(dtype)
														.arg(dname);
}

void Protocol::translate(const IGTLinkUSStatusMessage::Pointer body)
{
	this->writeNotSupportedMessage(body);
}

void Protocol::translate(const IGTLinkImageMessage::Pointer body)
{
	this->writeNotSupportedMessage(body);
}

} //namespace cx
