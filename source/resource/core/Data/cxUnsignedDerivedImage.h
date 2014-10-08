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


#ifndef CXUNSIGNEDDERIVEDIMAGE_H_
#define CXUNSIGNEDDERIVEDIMAGE_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxImage.h"

#define CALL_IN_WEAK_PTR(weak_base, func, defarg)       \
{                                                       \
    ImagePtr base = weak_base.lock();                   \
    if (!base)                                          \
        return defarg;                                  \
    return base->func();                                \
}

namespace cx
{
struct CoordinateSystem;

/** \brief Class that holds an unsigned version of a base Image
 *
 * This class provides read-only access to the base image only.
 * Intended for structures that requires unsigned input, such
 * as TextureSlice3DProxy.
 *
 * \ingroup cx_resource_core_data
 *   \date Feb 21, 2013
 *   \author christiana
 */
class cxResource_EXPORT UnsignedDerivedImage : public Image
{
    Q_OBJECT
public:
    static ImagePtr create(ImagePtr base);


    virtual RegistrationHistoryPtr get_rMd_History() { CALL_IN_WEAK_PTR(mBase, get_rMd_History, RegistrationHistoryPtr()); }
//    virtual QString getUid() const                   { CALL_IN_WEAK_PTR(mBase, getUid, QString()); }
    virtual QString getName() const                  { CALL_IN_WEAK_PTR(mBase, getName, QString()); }
	virtual QString getFilename() const              { CALL_IN_WEAK_PTR(mBase, getFilename, QString()); }
    virtual REGISTRATION_STATUS getRegistrationStatus() const { CALL_IN_WEAK_PTR(mBase, getRegistrationStatus, REGISTRATION_STATUS()); }
    virtual Transform3D get_rMd() const              { CALL_IN_WEAK_PTR(mBase, get_rMd, Transform3D()); }
    virtual Image::ShadingStruct getShading() const  { CALL_IN_WEAK_PTR(mBase, getShading, Image::ShadingStruct()); }
    virtual QDateTime getAcquisitionTime() const     { CALL_IN_WEAK_PTR(mBase, getAcquisitionTime, QDateTime()); }
    virtual QString getType() const                  { CALL_IN_WEAK_PTR(mBase, getType, QString()); }
    virtual QString getSpace()                       { CALL_IN_WEAK_PTR(mBase, getSpace, QString()); }
    virtual QString getParentSpace()                 { CALL_IN_WEAK_PTR(mBase, getParentSpace, QString()); }
    virtual DoubleBoundingBox3D boundingBox() const  { CALL_IN_WEAK_PTR(mBase, boundingBox, DoubleBoundingBox3D()); }
	virtual CoordinateSystem getCoordinateSystem();

    virtual QString getModality() const              { CALL_IN_WEAK_PTR(mBase, getModality, QString()); }
    virtual QString getImageType() const             { CALL_IN_WEAK_PTR(mBase, getImageType, QString()); }

private slots:
    void unsignedTransferFunctionsChangedSlot();
    void unsignedImageChangedSlot();
    void testSlot();

private:
    UnsignedDerivedImage(ImagePtr base);
    int findShift();
    vtkImageDataPtr convertImage();
    void convertTransferFunctions();

    boost::weak_ptr<Image> mBase;
};

}
#endif // CXUNSIGNEDDERIVEDIMAGE_H_
