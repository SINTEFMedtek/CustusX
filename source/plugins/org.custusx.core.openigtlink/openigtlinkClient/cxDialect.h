/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#ifndef CXDIALECT_H
#define CXDIALECT_H

#include "org_custusx_core_openigtlink_Export.h"

#include <QObject>

#include <boost/shared_ptr.hpp>

#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"
#include "igtlStringMessage.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"

#include "cxTransform3D.h"
#include "cxImage.h"
#include "cxProbeDefinition.h"

#define CX_OPENIGTLINK_CHANNEL_NAME "OpenIGTLink"

namespace cx
{
/**
 * @brief The Dialect class represents an interpretation of opentigtlink packages.
 */

class org_custusx_core_openigtlink_EXPORT Dialect : public QObject
{
    Q_OBJECT
public:
    explicit Dialect(QObject *parent = 0);

    virtual QString getName() const;
    virtual bool doCRC() const;

    virtual void translate(const igtl::TransformMessage::Pointer body);
    virtual void translate(const igtl::ImageMessage::Pointer body);
    virtual void translate(const igtl::StatusMessage::Pointer body);
    virtual void translate(const igtl::StringMessage::Pointer body);
    virtual void translate(const IGTLinkUSStatusMessage::Pointer body);
    virtual void translate(const IGTLinkImageMessage::Pointer body);

signals:
    void transform(QString devicename, Transform3D transform, double timestamp);
    void calibration(QString devicename, Transform3D calibration);
    void image(ImagePtr image);
    void igtlimage(IGTLinkImageMessage::Pointer igtlimage);
    void usstatusmessage(IGTLinkUSStatusMessage::Pointer msg);
    void probedefinition(QString devicename, ProbeDefinitionPtr definition);

protected:
	void writeNotSupportedMessage(igtl::MessageBase *body);
//	double extractTimeStamp(const igtl::MessageBase::Pointer body);

};
typedef boost::shared_ptr<Dialect> DialectPtr;

} //namespace cx
#endif // CXDIALECT_H
