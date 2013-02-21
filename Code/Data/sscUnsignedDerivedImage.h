// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCUNSIGNEDDERIVEDIMAGE_H
#define SSCUNSIGNEDDERIVEDIMAGE_H

#include "sscImage.h"

#define CALL_IN_WEAK_PTR(weak_base, func, defarg)       \
{                                                       \
    ImagePtr base = weak_base.lock();                   \
    if (!base)                                          \
        return defarg;                                  \
    return base->func();                                \
}

namespace ssc
{
/** \brief Class that holds an unsigned version of a base Image
 *
 * This class provides read-only access to the base image only.
 * Intended for structures that requires unsigned input, such
 * as TextureSlice3DProxy.
 *
 *   \ingroup sscData
 *   \date Feb 21, 2013
 *   \author christiana
 */
class UnsignedDerivedImage : public Image
{
    Q_OBJECT
public:
    static ImagePtr create(ImagePtr base);


    virtual RegistrationHistoryPtr get_rMd_History() { CALL_IN_WEAK_PTR(mBase, get_rMd_History, RegistrationHistoryPtr()); }
//    virtual QString getUid() const                   { CALL_IN_WEAK_PTR(mBase, getUid, QString()); }
    virtual QString getName() const                  { CALL_IN_WEAK_PTR(mBase, getName, QString()); }
    virtual QString getFilePath() const              { CALL_IN_WEAK_PTR(mBase, getFilePath, QString()); }
    virtual REGISTRATION_STATUS getRegistrationStatus() const { CALL_IN_WEAK_PTR(mBase, getRegistrationStatus, REGISTRATION_STATUS()); }
    virtual Transform3D get_rMd() const              { CALL_IN_WEAK_PTR(mBase, get_rMd, Transform3D()); }
    virtual Image::ShadingStruct getShading() const  { CALL_IN_WEAK_PTR(mBase, getShading, Image::ShadingStruct()); }
    virtual QDateTime getAcquisitionTime() const     { CALL_IN_WEAK_PTR(mBase, getAcquisitionTime, QDateTime()); }
    virtual QString getType() const                  { CALL_IN_WEAK_PTR(mBase, getType, QString()); }
    virtual QString getSpace()                       { CALL_IN_WEAK_PTR(mBase, getSpace, QString()); }
    virtual QString getParentSpace()                 { CALL_IN_WEAK_PTR(mBase, getParentSpace, QString()); }
    virtual DoubleBoundingBox3D boundingBox() const  { CALL_IN_WEAK_PTR(mBase, boundingBox, DoubleBoundingBox3D()); }
    virtual CoordinateSystem getCoordinateSystem()   { CALL_IN_WEAK_PTR(mBase, getCoordinateSystem, CoordinateSystem(csCOUNT)); }

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
#endif // SSCUNSIGNEDDERIVEDIMAGE_H
