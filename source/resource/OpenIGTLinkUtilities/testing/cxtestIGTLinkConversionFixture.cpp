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

#include "cxtestIGTLinkConversionFixture.h"

#include "catch.hpp"

vtkImageDataPtr IGTLinkConversionFixture::createRGBATestImage()
{
    Eigen::Array3i dim(512,512,1);
    int components = 4;
    vtkImageDataPtr retval = cx::generateVtkImageData(dim,
                                                      cx::Vector3D(0.5,0.6,0.7),
                                                      255, components);
//		int scalarSize = dim[0]*dim[1]*dim[2]*components;

    unsigned char* ptr = reinterpret_cast<unsigned char*>(retval->GetScalarPointer());

    for (unsigned z=0; z<dim[2]; ++z)
    {
        for (unsigned y=0; y<dim[1]; ++y)
        {
            for (unsigned x=0; x<dim[0]; ++x)
            {
                ptr[0] = 255;       // red
                ptr[1] = 0;         // green
                ptr[2] = x / 2;     // blue
                ptr[3] = 100;		// alpha
                ptr+=components;
            }
        }
    }

    return retval;
}

int IGTLinkConversionFixture::getValue(cx::ImagePtr data, int x, int y, int z)
{
    vtkImageDataPtr volume = data->getGrayScaleVtkImageData();
    int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
    return val;
}

Eigen::Array3i IGTLinkConversionFixture::getValue3i(cx::ImagePtr data, int x, int y, int z)
{
    vtkImageDataPtr volume = data->getBaseVtkImageData();
    unsigned char* ptr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
    return Eigen::Array3i(ptr[0], ptr[1], ptr[2]);
}

void IGTLinkConversionFixture::setValue(vtkImageDataPtr data, int x, int y, int z, unsigned char val)
{
    *reinterpret_cast<unsigned char*>(data->GetScalarPointer(x,y,z)) = val;
}

void IGTLinkConversionFixture::testDecodeEncodeColorImage(Val3VectorType values, QString colorFormat)
{
    QDateTime time = QDateTime::currentDateTime();
    vtkImageDataPtr rawImage = createRGBATestImage();

    QString coreUid = "my_uid";

    cx::ImagePtr input(new cx::Image(QString("%1 [%2]").arg(coreUid).arg(colorFormat), rawImage));
    input->setAcquisitionTime(time);

    cx::IGTLinkConversion converter;
	cx::IGTLinkImageMessage::Pointer msg = converter.encode(input);
    cx::ImagePtr output = converter.decode(msg);

    CHECK(output);
    CHECK(time == output->getAcquisitionTime());
    CHECK(coreUid == output->getUid());
    CHECK(cx::similar(Eigen::Array3i(input->getBaseVtkImageData()->GetDimensions()), Eigen::Array3i(output->getBaseVtkImageData()->GetDimensions())));
    CHECK(cx::similar(cx::Vector3D(input->getBaseVtkImageData()->GetSpacing()), cx::Vector3D(output->getBaseVtkImageData()->GetSpacing())));

    for (Val3VectorType::iterator iter=values.begin(); iter!=values.end(); ++iter)
    {
        Eigen::Array3i p = iter->first;
        Eigen::Array3i val = iter->second;
//		CHECK(similar(this->getValue3i(input, p[0], p[1], p[2]), val)); // cannot do this when changing color encoding
        CHECK(cx::similar(this->getValue3i(output, p[0], p[1], p[2]), val));
    }
}
