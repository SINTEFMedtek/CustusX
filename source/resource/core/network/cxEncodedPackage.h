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


#ifndef ENCODEDPACKAGE_H
#define ENCODEDPACKAGE_H

#include "cxResourceExport.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
//#include <QByteArray>
#include <QObject>

#include "igtlMessageHeader.h"

namespace cx
{

/**
 * Wrapper for shared memory area
 */
class cxResource_EXPORT EncodedPackage : public QObject
{
Q_OBJECT

public:
    struct ByteArray
    {
        void* pointer;
        int size;
    };
    typedef boost::shared_ptr<ByteArray> ByteArrayPtr;

public:
    EncodedPackage(void* pointer=NULL, int size=0);
    virtual ~EncodedPackage() {}
    virtual const ByteArrayPtr data() const = 0;
    //TODO rename these two...
    virtual void notifyDataArrived() = 0;
    virtual bool isFinishedWith() = 0;

signals:
    void dataArrived();

protected:
    ByteArrayPtr mData;

};
typedef boost::shared_ptr<EncodedPackage> EncodedPackagePtr;


//--------------------------------------------------------------
template<class TYPE>
class igtlEncodedPackage : public EncodedPackage
{
public:
    static EncodedPackagePtr create(typename TYPE::Pointer msg)
    {
        return EncodedPackagePtr(new igtlEncodedPackage<TYPE>(msg));
    }
    explicit igtlEncodedPackage(typename TYPE::Pointer msg) :
       EncodedPackage(msg->GetPackPointer(), msg->GetPackSize()),
       mMsg(msg),
       mCanBeOverWritten(true)
    {}
    virtual ~igtlEncodedPackage() {}

    const ByteArrayPtr data() const
    {
        return mData;
    }

    virtual void notifyDataArrived()
    {
        std::cout << "dataArrived()" << std::endl;
        mCanBeOverWritten = false;
        emit dataArrived();
    }

    virtual bool isFinishedWith()
    {
        return mCanBeOverWritten;
    }

    //private???
    typename TYPE::Pointer mMsg;
protected:
    bool mCanBeOverWritten;
};

class igtlEncodedBodyPackage : public igtlEncodedPackage<igtl::MessageBase>
{
public:
    static EncodedPackagePtr create(igtl::MessageBase::Pointer msg)
    {
        return EncodedPackagePtr(new igtlEncodedBodyPackage(msg));
    }
    explicit igtlEncodedBodyPackage(igtl::MessageBase::Pointer msg) :
        igtlEncodedPackage(msg)
    {
        mMsg = msg;
        mCanBeOverWritten = true;
        mData->pointer = msg->GetPackBodyPointer();
        mData->size = msg->GetPackBodySize();
    }
    virtual ~igtlEncodedBodyPackage() {}
};


//--------------------------------------------------------------
template<class TYPE>
static EncodedPackagePtr createEncodedPackage(typename TYPE::Pointer msg)
{
    return EncodedPackagePtr(new igtlEncodedPackage<TYPE>(msg));
}

} //namespace cx
#endif // ENCODEDPACKAGE_H
