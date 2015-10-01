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


#ifndef CXPROTOCOL_H
#define CXPROTOCOL_H

#include "org_custusx_core_openigtlink_Export.h"

#include <QObject>

#include <boost/shared_ptr.hpp>

#include "cxTransform3D.h"
#include "cxImage.h"
#include "cxProbeDefinition.h"

namespace cx
{

class Pack : public QObject
{
    Q_OBJECT

signals:
    void dataArrived();

public:
    Pack(void* pointer=NULL, int size=0) :
        pointer(pointer),
        size(size),
        canBeOverWritten(true)
    {}

    void notifyDataArrived()
    {
        std::cout << "dataArrived()" << std::endl;
        canBeOverWritten = false;
        emit dataArrived();
    }

    bool isFinishedWith()
    {
        return canBeOverWritten;
    }

    void* pointer;
    int size;

private:
    bool canBeOverWritten;

};
typedef boost::shared_ptr<Pack> PackPtr;


/** Wraps anything that can be viewed as a const QByteArray.
 */
class EncodedPackage
{
public:
	virtual ~EncodedPackage() {}
	virtual const QByteArray data() const = 0;
};
typedef boost::shared_ptr<EncodedPackage> EncodedPackagePtr;

/**
 * @brief The Dialect class represents an interpretation of opentigtlink packages.
 */

class org_custusx_core_openigtlink_EXPORT Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = 0);

    virtual QString getName() const;
    virtual PackPtr getPack();
    virtual bool readyToReceiveData() = 0;
	virtual EncodedPackagePtr encode(ImagePtr image) = 0;
	virtual EncodedPackagePtr encode(MeshPtr data) = 0;

protected slots:
    virtual void processPack() = 0;

signals:
    void transform(QString devicename, Transform3D transform, double timestamp);
    void calibration(QString devicename, Transform3D calibration);
    void image(ImagePtr image);
	void mesh(MeshPtr mesh);
    void probedefinition(QString devicename, ProbeDefinitionPtr definition);

protected:
    QMutex mPackMutex;
    PackPtr mPack;

};
typedef boost::shared_ptr<Protocol> ProtocolPtr;

} //namespace cx
#endif // CXPROTOCOL_H
