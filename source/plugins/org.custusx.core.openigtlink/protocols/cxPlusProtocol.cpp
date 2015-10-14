#include "cxPlusProtocol.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"
#include "cxIGTLinkConversionBase.h"

namespace cx{

PlusProtocol::PlusProtocol() :
    mCalibrationKeyword("CalibrationTo"),
    mProbeToTrackerName("ProbeToTracker"), //set in the PlusServer config file
    mLastKnownOriginalTimestamp(-1),
    mLastKnownLocalTimestamp(-1)
{
    /* Rotation from igtl coordinate system to
     * custusxs tool coordination system definition:
     * 0 1 0 0
     * 0 0 1 0
     * 1 0 0 0
     * 0 0 0 1
     */
    igtltool_M_custustool = Transform3D::Identity();
    igtltool_M_custustool.matrix() = Eigen::Matrix4d::Constant(0);
    igtltool_M_custustool(2, 0) = 1;
    igtltool_M_custustool(0, 1) = 1;
    igtltool_M_custustool(1, 2) = 1;
    igtltool_M_custustool(3,3) = 1;
}

QString PlusProtocol::getName() const
{
    return "PlusServer";
}

void PlusProtocol::translate(const igtl::TransformMessage::Pointer body)
{
    CX_LOG_DEBUG() << "Transform incoming to plusprotocol";
    QString deviceName = body->GetDeviceName();
    this->registerTransformDeviceName(deviceName);

    IGTLinkConversion converter;
    Transform3D matrix = converter.decode(body);

    if(this->isCalibration(deviceName))
    {
        Transform3D s_M_igtltool = matrix;
        Transform3D s_M_custustool = s_M_igtltool * igtltool_M_custustool;
        Transform3D sMt = s_M_custustool;
        QString calibrationBelongsToDeviceName = this->findDeviceForCalibration(deviceName);
        if(calibrationBelongsToDeviceName != "NOT_FOUND")
        {
            emit calibration(calibrationBelongsToDeviceName, sMt);
        }
    }
    else
    {
		IGTLinkConversionBase baseConverter;
		double timestamp_ms = baseConverter.decode_timestamp(body).toMSecsSinceEpoch();
		timestamp_ms = this->getSyncedTimestampForTransformsAndImages(timestamp_ms);
        Transform3D prMs = matrix;
        emit transform(deviceName, prMs, timestamp_ms);
    }
}

void PlusProtocol::translate(const igtl::ImageMessage::Pointer body)
{
    CX_LOG_DEBUG() << "Image incoming to plusprotocol";
    //DIMENSION
    int x = 0;
    int y = 1;
    int z = 2;

    //There seems to be a bug in the received images spacing from the plusserver
    /*
    float wrong_spacing[3];
    body->GetSpacing(wrong_spacing);
    float right_spacing[3];
    right_spacing[x] = wrong_spacing[x];
    right_spacing[y] = wrong_spacing[z];
    right_spacing[z] = 1;
    body->SetSpacing(right_spacing);
    */

    int dimensions_p[3];
    body->GetDimensions(dimensions_p);
    float spacing[3];
    body->GetSpacing(spacing);
    int extent_p[3];
    extent_p[x] = dimensions_p[x]-1;
    extent_p[y] = dimensions_p[y]-1;
    extent_p[z] = dimensions_p[z]-1;

    //IMAGE
    IGTLinkConversion converter;
    ImagePtr theImage = converter.decode(body);

	IGTLinkConversionBase baseConverter;
	double timestamp_ms = baseConverter.decode_timestamp(body).toMSecsSinceEpoch();
	timestamp_ms = this->getSyncedTimestampForTransformsAndImages(timestamp_ms);
    theImage->setAcquisitionTime(QDateTime::fromMSecsSinceEpoch(timestamp_ms));
    emit image(theImage);

    //PROBEDEFINITION
    ProbeDefinitionPtr definition(new ProbeDefinition);
    definition->setUseDigitalVideo(true);
    definition->setType(ProbeDefinition::tLINEAR);
    definition->setSpacing(Vector3D(spacing[x], spacing[y], spacing[z]));
    definition->setSize(QSize(dimensions_p[x], dimensions_p[y]));
    definition->setOrigin_p(Vector3D(dimensions_p[x]/2, 0, 0));
    double depthstart_mm = 0;
    double depthend_mm = extent_p[y]*spacing[y];
    double width_mm = extent_p[x]*spacing[x];
    definition->setSector(depthstart_mm, depthend_mm, width_mm);
    definition->setClipRect_p(DoubleBoundingBox3D(0, extent_p[x], 0, extent_p[y], 0, extent_p[z]));

    emit probedefinition(mProbeToTrackerName, definition);
}

void PlusProtocol::translate(const igtl::StringMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString string = converter.decode(body);
    if(string.contains("CommandReply", Qt::CaseInsensitive))
        CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << string;
    //else
    //    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << string;
}

double PlusProtocol::getCurrentTimestamp() const
{
    double current_timestamp_ms = QDateTime::currentDateTime().toMSecsSinceEpoch();
    return current_timestamp_ms;
}
double PlusProtocol::getSyncedTimestampForTransformsAndImages(double currentOriginalTimestamp)
{
    double retval = 0;

    if(currentOriginalTimestamp != mLastKnownOriginalTimestamp)
    {
        double currentLocalTimestamp = this->getCurrentTimestamp();
        mLastKnownOriginalTimestamp = currentOriginalTimestamp;
        mLastKnownLocalTimestamp = currentLocalTimestamp;
    }
    retval = mLastKnownLocalTimestamp;
    return retval;
}

void PlusProtocol::registerTransformDeviceName(QString deviceName)
{
    if(!mKnownTransformDeviceNames.contains(deviceName))
        mKnownTransformDeviceNames << deviceName;
}

bool PlusProtocol::isCalibration(QString deviceName) const
{
    if(deviceName.contains(mCalibrationKeyword))
        return true;
    else
        return false;
}

QString PlusProtocol::findDeviceForCalibration(QString calibrationDeviceName) const
{
    QString partialDeviceName = this->extractDeviceNameFromCalibrationDeviceName(calibrationDeviceName);
    QString calibrationBelongsToDeviceName = this->findRegisteredTransformDeviceNameThatContains(partialDeviceName);
    return calibrationBelongsToDeviceName;
}

QString PlusProtocol::extractDeviceNameFromCalibrationDeviceName(QString calibrationDeviceName) const
{
    QString retval = calibrationDeviceName.remove(mCalibrationKeyword);
    return retval;
}

QString PlusProtocol::findRegisteredTransformDeviceNameThatContains(QString deviceName) const
{
    QString retval("NOT_FOUND");
    deviceName.remove("X");
    QRegExp rx(deviceName+"*");
    rx.setPatternSyntax(QRegExp::Wildcard);
    int foundAtIndex = mKnownTransformDeviceNames.indexOf(rx);
    if(foundAtIndex != -1)
        retval = mKnownTransformDeviceNames[foundAtIndex];

    return retval;

}
} //namespace cx
