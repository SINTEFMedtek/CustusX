/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
