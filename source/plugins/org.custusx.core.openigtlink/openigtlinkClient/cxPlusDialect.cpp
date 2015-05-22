#include "cxPlusDialect.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"

namespace cx{

PlusDialect::PlusDialect() :
    mProbeToTrackerName("ProbeToTracker") //set in the PlusServer config file
{
}


QString PlusDialect::getName() const
{
    return "PlusServer";
}

void PlusDialect::translate(const igtl::ImageMessage::Pointer body)
{
    //DIMENSION
    int x = 0;
    int y = 1;
    int z = 2;

    //Snakke med Daniel!
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
    emit image(theImage);

    //CALIBRATION
    //OpenIGTLink defines the origo of the image in the center(c)¸
    //because of this we first need to translate the calibration matrix
    //into our images origo (v) which i is upper left
    Transform3D vMc = Transform3D::Identity();
    //vMc.translation() = Eigen::Vector3d(-dimensions_p[x]*spacing[x]*0.5*0, dimensions_p[y]*spacing[y]*0.5, -dimensions_p[z]*spacing[z]*0.5*0);
    vMc.translation() = Eigen::Vector3d(0, dimensions_p[y]*spacing[y]*0.5, 0);
    CX_LOG_DEBUG() << "translated vMc " << vMc;
    Transform3D sMt = converter.decode_image_matrix(body);
    CX_LOG_DEBUG() << "sMt before translation " << sMt;
    sMt = vMc*sMt;
    CX_LOG_DEBUG() << "translated sMt " << sMt;
    emit calibration(mProbeToTrackerName, sMt);

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

void PlusDialect::translate(const igtl::StringMessage::Pointer body)
{
    IGTLinkConversion converter;
    QString string = converter.decode(body);
    if(string.contains("CommandReply", Qt::CaseInsensitive))
        CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << string;
    //else
    //    CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << string;
}

} //namespace cx
