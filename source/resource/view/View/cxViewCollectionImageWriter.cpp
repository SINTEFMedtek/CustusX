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
#include "cxViewCollectionImageWriter.h"

#include <QPixmap>
#include "cxPatientModelService.h"
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QApplication>
#include "cxReporter.h"
#include "boost/bind.hpp"
#include <QScreen>
#include <QVBoxLayout>
#include "cxViewService.h"
//#include "cxSecondaryViewLayoutWindow.h"
#include "cxViewCollectionWidget.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkPNGWriter.h"
#include "vtkUnsignedCharArray.h"
#include <QPainter>
#include "cxVolumeHelpers.h"
#include "vtkImageImport.h"
#include <QTime>

namespace cx
{


ViewCollectionImageWriter::ViewCollectionImageWriter(ViewCollectionWidget* widget) : mWidget(widget)
{

}

vtkImageDataPtr ViewCollectionImageWriter::grab()
{
    std::vector<ViewPtr> views = mWidget->getViews();
    Eigen::Array3i target_size(mWidget->width(), mWidget->height(), 1);
    vtkImageDataPtr target = generateVtkImageData(target_size, Vector3D(1,1,1), 150, 3);

	for (unsigned i=0; i<views.size(); ++i)
	{
        vtkImageDataPtr vtkImage = this->view2vtkImageData(views[i]);
        QPoint pos = mWidget->getPosition(views[i]);
        this->drawImageAtPos(target, vtkImage, pos);
    }

    return target;
}

void ViewCollectionImageWriter::drawImageAtPos(vtkImageDataPtr target, vtkImageDataPtr image, QPoint pos)
{
    unsigned char* src = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
    unsigned char* dst = reinterpret_cast<unsigned char*>(target->GetScalarPointer());
    Eigen::Array3i dim_src(image->GetDimensions());
    int depth = 3;
    CX_ASSERT(target->GetNumberOfScalarComponents()==depth);
    CX_ASSERT(image->GetNumberOfScalarComponents()==depth);

    for (int y=0; y<dim_src[1]; ++y)
	{
        unsigned char* src = reinterpret_cast<unsigned char*>(image->GetScalarPointer(0,y,0));
        unsigned char* dst = reinterpret_cast<unsigned char*>(target->GetScalarPointer(pos.x(),pos.y()+y,0));
        memcpy(dst, src, dim_src[0]*depth);
    }

}

// the vtk canonical way
//vtkImageDataPtr ViewCollectionImageWriter::view2vtkImageData(ViewPtr view)
//{
//    Eigen::Array4d vp(view->getRenderer()->GetViewport());
//	vtkWindowToImageFilterPtr w2i = vtkWindowToImageFilterPtr::New();
//    w2i->ShouldRerenderOff();
//	w2i->SetInput(view->getRenderWindow());
//	w2i->SetViewport(vp.data());
//	w2i->SetReadFrontBuffer(false);
//    CX_LOG_CHANNEL_DEBUG("CA") << "   - 2";
//    w2i->Update();
//    CX_LOG_CHANNEL_DEBUG("CA") << "   - 3 -upd";
//    vtkImageDataPtr image = w2i->GetOutput();
//    return image;
//}

// alternative to vtkWindowToImageFilter, effectively reimplementing parts of that filter (for debugging speed)
vtkImageDataPtr ViewCollectionImageWriter::view2vtkImageData(ViewPtr view)
{
	vtkRenderWindowPtr renderWindow = view->getRenderWindow();
	vtkRendererPtr renderer = view->getRenderer();
    Eigen::Array4d vp(renderer->GetViewport()); // need the viewport in pixels

    Eigen::Array2i origin(renderer->GetOrigin());
    Eigen::Array2i size(renderer->GetSize());
    Eigen::Array<int,6,1> extent;
    extent[0] = 0;
    extent[1] = size[0]-1;
    extent[2] = 0;
    extent[3] = size[1]-1;
    extent[4] = 0;
    extent[5] = 0;
    int frontBuffer = false;

    unsigned char *pixels;
//    QTime qtimer = QTime::currentTime();

//    CX_LOG_CHANNEL_DEBUG("CA") << "   renderWindow->GetPixelData0";
    renderWindow->MakeCurrent();
//    CX_LOG_CHANNEL_DEBUG("CA") << "   after makecurrent";
    pixels = renderWindow->GetPixelData(origin[0], origin[1], origin[0]+size[0]-1, origin[1]+size[1]-1, frontBuffer);
//    CX_LOG_CHANNEL_DEBUG("CA") << "   renderWindow->GetPixelData2";

//    std::cout << "ViewCollectionImageWriter::view2vtkImageData qtimer ms=" << qtimer.msecsTo(QTime::currentTime()) << std::endl;

    typedef vtkSmartPointer<vtkImageImport> vtkImageImportPtr;
    vtkImageImportPtr import = vtkImageImportPtr::New();
    import->SetDataScalarTypeToUnsignedChar();
    import->SetNumberOfScalarComponents(3);
    import->SetDataExtent(extent.data());
    import->SetWholeExtent(extent.data());
    bool takeOwnership = true;
    import->SetImportVoidPointer(pixels, !takeOwnership);
    import->Update();
    return import->GetOutput();
}


QImage ViewCollectionImageWriter::vtkImageData2QImage(vtkImageDataPtr input)
{
    CX_ASSERT(input->GetNumberOfScalarComponents()==3);
    CX_ASSERT(input->GetScalarType() == VTK_UNSIGNED_CHAR);

    unsigned char* ptr = reinterpret_cast<unsigned char*>(input->GetScalarPointer());
	Eigen::Array3i dim(input->GetDimensions());

    // important: input the line length. This will not copy the buffer.
    QImage retval(ptr, dim[0], dim[1], dim[0]*3, QImage::Format_RGB888);
    // copy contents into image, then flip according to conventions vtk<->qt.
    return retval.copy().mirrored(false, true);
}


} // namespace cx
