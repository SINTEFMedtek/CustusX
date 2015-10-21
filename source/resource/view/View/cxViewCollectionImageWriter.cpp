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

namespace cx
{


ViewCollectionImageWriter::ViewCollectionImageWriter(ViewCollectionWidget* widget) : mWidget(widget)
{

}

QImage ViewCollectionImageWriter::grab()
{
	std::vector<ViewPtr> views = mWidget->getViews();
	QSize size = mWidget->size();
	CX_LOG_CHANNEL_DEBUG("CA") << "grab layout size " << size.width() << "," << size.height();

//	QImage target(QSize(320, 568), QImage::Format_RGB888);
	QImage target(size, QImage::Format_RGB888);
	target.fill(QColor("red"));
//	QPainter painter(&target);

	LayoutRegion totalRegion = mWidget->getLayoutRegion(views[0]->getUid());
	for (unsigned i=1; i<views.size(); ++i)
	{
		totalRegion = merge(totalRegion, mWidget->getLayoutRegion(views[i]->getUid()));
	}

//	mWidget->render();

	CX_LOG_CHANNEL_DEBUG("CA") << "views: " << views.size();
	for (unsigned i=0; i<views.size(); ++i)
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "view " << i;
		vtkImageDataPtr vtkImage = this->view2vtkImageData(views[i]);

//		vtkPNGWriterPtr pngWriter = vtkPNGWriterPtr::New();
//		pngWriter->SetFileName("/Users/christiana/Patients/Laboratory/2015-10-20_19-06_Laboratory_15.cx3/tst.png");
//		pngWriter->SetInputData(vtkImage);
//		pngWriter->Write();

//		QImage qImage = vtkImageData2QImage(vtkImage);
//		qImage = qImage.mirrored(false, true);

		this->printViewport(views[i]->getRenderer());

		LayoutRegion region = mWidget->getLayoutRegion(views[i]->getUid());
		CX_LOG_CHANNEL_DEBUG("CA") << QString("  region %1 %2, %3 %4 ")
									  .arg(region.pos.col).arg(region.pos.row)
									  .arg(region.span.col).arg(region.span.row);

		QPoint pos;
		pos.setX(double(region.pos.col) / double(totalRegion.span.col) * target.size().width());
		pos.setY(double(region.pos.row) / double(totalRegion.span.row) * target.size().height());
		CX_LOG_CHANNEL_DEBUG("CA") << "  pos: " << pos.x() << ", " << pos.y();

		this->drawImageAtPos(target, vtkImage, pos);

//		painter.drawImage(pos, qImage);
	}

	return target;
}

void ViewCollectionImageWriter::drawImageAtPos(QImage& target, vtkImageDataPtr image, QPoint pos)
{
	// draw each line in image to target, at pos
//	unsigned char* src = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
	unsigned char* dst = target.bits();
	Eigen::Array3i dim(image->GetDimensions());
	int depth = 3;

	CX_LOG_CHANNEL_DEBUG("CA") << "    target: " << target.width() << "," << target.height() << " - " << target.depth();
	CX_LOG_CHANNEL_DEBUG("CA") << "    dims:   " << dim;
	CX_LOG_CHANNEL_DEBUG("CA") << "    pos:    " << pos.x() << ", " << pos.y();
//	image->Print(std::cout);
	std::vector<uchar> flat(dim[0]*depth, 100);

	CX_LOG_CHANNEL_DEBUG("CA") << "    bytesperline:   " <<target.bytesPerLine() ;
	CX_LOG_CHANNEL_DEBUG("CA") << "    assumed bytes/line:   " << depth * target.width() ;

	for (int y=0; y<dim[1]; ++y)
	{
		uchar* dstline = target.scanLine(pos.y()+y) + depth * pos.x();
//		uchar* dstline = dst + depth * (target.width()*(pos.y()+y) + pos.x());
//		uchar* srcline = src + (dim[0]*depth*y); // increment?
		unsigned char* srcline = reinterpret_cast<unsigned char*>(image->GetScalarPointer(0,y,0));
//		memcpy(dstline, srcline, dim[0]*depth);
		memcpy(dstline, &*flat.begin(), dim[0]*depth);
	}

}

void ViewCollectionImageWriter::printViewport(vtkRendererPtr renderer)
{
	Eigen::Array4d vp(renderer->GetViewport());

	CX_LOG_CHANNEL_DEBUG("CA") << "  norm viewport" << vp;
	renderer->NormalizedViewportToViewport(vp.data()[0], vp.data()[1]);
	renderer->NormalizedViewportToViewport(vp.data()[2], vp.data()[3]);
	CX_LOG_CHANNEL_DEBUG("CA") << "  viewport" << vp;
}

vtkImageDataPtr ViewCollectionImageWriter::view2vtkImageData(ViewPtr view)
{
	Eigen::Array4d vp(view->getRenderer()->GetViewport());
	vtkWindowToImageFilterPtr w2i = vtkWindowToImageFilterPtr::New();
	w2i->SetInput(view->getRenderWindow());
	w2i->SetViewport(vp.data());
	w2i->SetReadFrontBuffer(false);
	w2i->Update();
	CX_LOG_CHANNEL_DEBUG("CA") << "Updated view " << vp;
	vtkImageDataPtr image = w2i->GetOutput();
	return image;
}

QImage ViewCollectionImageWriter::vtkImageData2QImage(vtkImageDataPtr input)
{
//	input->Print(std::cout);
	unsigned char* ptr = reinterpret_cast<unsigned char*>(input->GetScalarPointer());
	Eigen::Array3i dim(input->GetDimensions());
//	int len = image->GetNumberOfScalarComponents() * image->GetScalarSize() * dim[0] * dim[1];

	QImage retval(ptr, dim[0], dim[1], QImage::Format_RGB888);
	return retval;
}


} // namespace cx
