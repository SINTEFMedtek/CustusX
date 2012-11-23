// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXBINARYTHINNINGIMAGEFILTER3DFILTER_H
#define CXBINARYTHINNINGIMAGEFILTER3DFILTER_H

#include "cxFilterImpl.h"

#include "sscColorDataAdapterXml.h"

namespace cx
{

/** Filter implementation of the itk::BinaryThinningImageFilter3D
 *
 *
 * \ingroup cx
 * \date 11 22, 2012
 * \author christiana
 */
class BinaryThinningImageFilter3DFilter : public FilterImpl
{
    Q_OBJECT

public:
    virtual ~BinaryThinningImageFilter3DFilter() {}

    virtual QString getType() const;
    virtual QString getName() const;
    virtual QString getHelp() const;

    virtual bool preProcess();
    virtual bool execute();
    virtual void postProcess();

    ssc::ColorDataAdapterPtr getColorOption(QDomElement root);

protected:
    virtual void createOptions(QDomElement root);
    virtual void createInputTypes();
    virtual void createOutputTypes();

private:
    vtkImageDataPtr mRawResult;
};


} // namespace cx

#endif // CXBINARYTHINNINGIMAGEFILTER3DFILTER_H
