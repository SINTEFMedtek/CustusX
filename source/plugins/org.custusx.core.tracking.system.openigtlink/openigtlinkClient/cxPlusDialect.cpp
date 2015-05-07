#include "cxPlusDialect.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"

namespace cx{


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

    //There seems to be a bug in the received images spacing from the plusserver
    float hack_spacing[3];
    body->GetSpacing(hack_spacing);
    float new_spacing[3];
    new_spacing[x] = hack_spacing[x];
    new_spacing[y] = hack_spacing[z];
    new_spacing[z] = 1;
    body->SetSpacing(new_spacing);

    //IMAGE
    IGTLinkConversion converter;
    ImagePtr theImage = converter.decode(body);
    emit image(theImage);

    //CALIBRATION
    QString devicename = "ProbeToTracker"; //TODO remove hardcoded devicename
    Transform3D sMt = converter.decode_image_matrix(body);
    emit calibration(devicename, sMt);

    //PROBEDEFINITION
    int dimensions_p[3];
    body->GetDimensions(dimensions_p);
    float spacing[3];
    body->GetSpacing(spacing);
    int extent_p[3];
    extent_p[x] = dimensions_p[x]-1;
    extent_p[y] = dimensions_p[y]-1;
    extent_p[z] = dimensions_p[z]-1;

    ProbeDefinitionPtr definition(new ProbeDefinition);
    definition->setUseDigitalVideo(true);
    definition->setType(ProbeDefinition::tLINEAR);
    definition->setSpacing(Vector3D(spacing[x], spacing[y], spacing[z]));//(Vector3D(0.087, 0.087, 1));
    definition->setSize(QSize(dimensions_p[x], dimensions_p[y]));//(QSize(446, 460));
    definition->setOrigin_p(Vector3D(dimensions_p[x]/2, 0, 0));//(Vector3D(223, 0, 0));
    double depthstart_mm = 0;
    double depthend_mm = extent_p[y]*spacing[y];
    double width_mm = extent_p[x]*spacing[x];
    definition->setSector(depthstart_mm, depthend_mm, width_mm);//(0, )
    definition->setClipRect_p(DoubleBoundingBox3D(0, extent_p[x], 0, extent_p[y], 0, extent_p[z]));//(DoubleBoundingBox3D(0,445, 0,459, 0, 1));

    emit probedefinition(devicename, definition);
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
