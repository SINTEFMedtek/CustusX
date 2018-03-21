/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestRenderTester.h"

#include "catch.hpp"

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkImageDifference.h>
#include <vtkImageAppendComponents.h>
#include <vtkRendererCollection.h>
#include <vtkImageImport.h>

#include "cxBoundingBox3D.h"
#include "cxView.h"
#include "cxTypeConversions.h"
#include "cxDataReaderWriter.h"
#include "cxtestUtilities.h"
#include "cxReporter.h"

#include "cxUtilHelpers.h"


#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkTextProperty.h"
#include "vtkRendererCollection.h"


typedef vtkSmartPointer<class vtkProp> vtkPropPtr;
typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;
typedef vtkSmartPointer<class vtkPNGWriter> vtkPNGWriterPtr;
typedef vtkSmartPointer<class vtkPNGReader> vtkPNGReaderPtr;

typedef vtkSmartPointer<class vtkImageDifference> vtkImageDifferencePtr;
typedef vtkSmartPointer<class vtkImageClip> vtkImageClipPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;


namespace cxtest
{


RenderTesterPtr RenderTester::create()
{
	return RenderTesterPtr(new RenderTester());
}

RenderTesterPtr RenderTester::create(vtkRenderWindowPtr renderWindow)
{
	return RenderTesterPtr(new RenderTester(renderWindow));
}

RenderTesterPtr RenderTester::create(vtkRenderWindowPtr renderWindow, vtkRendererPtr renderer)
{
	return RenderTesterPtr(new RenderTester(renderWindow, renderer));
}

//RenderTesterPtr RenderTester::create(cx::RepPtr rep, const unsigned int viewAxisSize)
//{
//	return RenderTesterPtr(new RenderTester(rep, viewAxisSize));
//}

RenderTester::RenderTester() :
	mImageErrorThreshold(100.0),
	mBorderOffset(2)
{
//	mImageErrorThreshold = 1.0;
//	mImageErrorThreshold = 100.0;
//	mBorderOffset = 2;
	//		mView = new ViewWidget(NULL);
	//		// set some defaults nice for testing:
	//		this->getView()->setBackgroundColor(QColor::fromRgbF(0.1, 0.4, 0.2));
	////		this->getView()->getRenderWindow()->SetSize(301,300);
	//		this->getView()->setGeometry(QRect(0,0,120,120));

	mRenderWindow = vtkRenderWindowPtr::New();
	mRenderer = vtkRendererPtr::New();
	mRenderer->SetBackground(0.1,0.4,0.2); //??
	mRenderer->GetActiveCamera()->SetParallelProjection(true);

	mRenderWindow->AddRenderer(mRenderer);
	mRenderWindow->SetSize(120,120);
}

RenderTester::RenderTester(vtkRenderWindowPtr renderWindow) :
	mRenderWindow(renderWindow),
	mRenderer(renderWindow->GetRenderers()->GetFirstRenderer()),
	mImageErrorThreshold(100.0),
	mBorderOffset(2)
{
//	CX_ASSERT(renderWindow->GetRenderers()->GetNumberOfItems()==1);
}

RenderTester::RenderTester(vtkRenderWindowPtr renderWindow, vtkRendererPtr renderer) :
	mRenderWindow(renderWindow),
	mRenderer(renderer),
	mImageErrorThreshold(100.0),
	mBorderOffset(2)
{
}

//RenderTester::RenderTester(cx::RepPtr rep, const unsigned int viewAxisSize) :
//	mImageErrorThreshold(100.0),
//	mBorderOffset(2)
//{
//	mLayoutWidget.reset(cx::LayoutWidget::createViewWidgetLayout().data());
//	mView = mLayoutWidget->addView(cx::View::VIEW, cx::LayoutRegion(0,0));
//	mView->addRep(rep);
//	mLayoutWidget->resize(viewAxisSize,viewAxisSize);
//	mLayoutWidget->show();

//	mRenderWindow = mView->getRenderWindow();
//	mRenderer = mView->getRenderer();

////??	mRenderWindow->SetSize(viewAxisSize,viewAxisSize);

//}

void RenderTester::setImageErrorThreshold(double value)
{
		mImageErrorThreshold = value;
}

void RenderTester::addProp(vtkPropPtr prop)
{
	mRenderer->AddViewProp(prop);
}

void RenderTester::renderToFile(QString filename)
{
	vtkImageDataPtr image = this->renderToImage();
	this->writeToPNG(image, filename);
}

void RenderTester::alignRenderWindowWithImage(vtkImageDataPtr input)
{
	cx::DoubleBoundingBox3D bounds(input->GetBounds());
	mRenderer->ResetCamera(bounds.data());

	Eigen::Array3i dim(input->GetDimensions());
	mRenderWindow->SetSize(dim[0], dim[1]);

	// No idea why we must use bounds+1 here ... should be vp height in world space.
	// Set/Get the scaling used for a parallel projection, i.e. the height of the viewport in world-coordinate distances.
	//		std::cout << "(bounds.range()[1]) " << (bounds.range()[1]) << std::endl;
//	mRenderer->GetActiveCamera()->SetParallelScale((bounds.range()[1]+1)/2);
	mRenderer->GetActiveCamera()->SetParallelScale((bounds.range()[1])/2);
}

void RenderTester::resetCamera()
{
	mRenderer->ResetCamera();
}

vtkImageDataPtr RenderTester::renderToImage()
{
	mRenderWindow->Render();
	// extra render, as suggested by http://read.pudn.com/downloads115/ebook/487640/VTK/Common/Testing/Cxx/vtkRegressionTestImage.h__.htm
	// tests show that black windows appear less often with two renders.
	mRenderWindow->Render();

	cx::sleep_ms(200);

	return this->getImageFromRenderWindow();
}

/*removed for testing fails on win
vtkImageDataPtr RenderTester::getImageFromRenderWindow()
{
	mRenderWindow->Render();
	mRenderWindow->Render();

	vtkImageDataPtr retval = vtkImageDataPtr::New();

//	std::cout << "mapped: " << mRenderWindow->GetMapped() << std::endl;
//	mRenderWindow->Render();
//	std::cout << "erase: " << mRenderWindow->GetErase() << std::endl;
//	mRenderWindow->SetErase(false);
//	mRenderWindow->SetDoubleBuffer(false);
	Eigen::Vector2i size(mRenderWindow->GetSize());
	bool useFrontBuffer = true; // false gives lots of garbage in the image - at least on mac
	void* rawPointer = mRenderWindow->GetPixelData(0, 0, size[0]-1, size[1]-1, useFrontBuffer);

	vtkImageImportPtr import = vtkImageImportPtr::New();

//	std::cout << "size: " << size << std::endl;
	import->SetImportVoidPointer(rawPointer, 0);
	import->SetDataScalarTypeToUnsignedChar();
	import->SetDataSpacing(1, 1, 1);
	import->SetNumberOfScalarComponents(3);
	import->SetWholeExtent(0, size[0]-1, 0, size[1]-1, 0, 0);
	import->SetDataExtentToWholeExtent();
	import->Update();

	return import->GetOutput();
}
*/

vtkImageDataPtr RenderTester::getImageFromRenderWindow()
{
	vtkWindowToImageFilterPtr windowToImageFilter = vtkWindowToImageFilterPtr::New();
//	windowToImageFilter->ShouldRerenderOff();
#ifdef CX_WINDOWS
	// for windows: read the back buffer. Determined experimentally, no idea why. Ubuntu/AMD: Must use front buffer
	windowToImageFilter->SetReadFrontBuffer(false); // might give less interf erence from other windows...?
#endif
//	mRenderWindow->Render();
//	mRenderWindow->Render();
	windowToImageFilter->SetInput(mRenderWindow);
	windowToImageFilter->Modified();
	windowToImageFilter->Update();

	return windowToImageFilter->GetOutput();
}

void RenderTester::writeToPNG(vtkImageDataPtr image, QString filename)
{
	vtkPNGWriterPtr pngWriter = vtkPNGWriterPtr::New();
	pngWriter->SetFileName(filename.toStdString().c_str());
	pngWriter->SetInputData(image);
	pngWriter->Write();
}

vtkImageDataPtr RenderTester::readFromFile(QString filename)
{
	return cx::DataReaderWriter().loadVtkImageData(filename);
}

vtkImageDataPtr RenderTester::readFromPNG(QString filename)
{
	vtkPNGReaderPtr pngReader = vtkPNGReaderPtr::New();
	pngReader->SetFileName(filename.toStdString().c_str());
	pngReader->Update();
	return pngReader->GetOutput();
}

void RenderTester::enterRunLoop()
{
	// Setup render window interactor
	vtkRenderWindowInteractorPtr renderWindowInteractor = vtkRenderWindowInteractorPtr::New();
	renderWindowInteractor->SetRenderWindow(mRenderWindow);
	// Render and start interaction
	mRenderWindow->Render();
	renderWindowInteractor->Start();
}

vtkImageDataPtr RenderTester::clipImage(vtkImageDataPtr input)
{
	vtkImageClipPtr imageClip = vtkImageClipPtr::New();
	imageClip->SetClipData(1);
	imageClip->SetInputData(input);

	int* wExt1 = input->GetExtent();
	imageClip->SetOutputWholeExtent(wExt1[0] + this->mBorderOffset,
									wExt1[1] - this->mBorderOffset,
									wExt1[2] + this->mBorderOffset,
									wExt1[3] - this->mBorderOffset,
									wExt1[4],
									wExt1[5]);
	imageClip->Update();
	return imageClip->GetOutput();
}

bool RenderTester::equalExtent(vtkImageDataPtr input1, vtkImageDataPtr input2)
{
	int ext1[6], ext2[6];
//	input1->Update();
	input1->GetExtent(ext1);
//	input2->Update();
	input2->GetExtent(ext2);

	if ((ext2[1]-ext2[0]) == (ext1[1]-ext1[0]) &&
		(ext2[3]-ext2[2]) == (ext1[3]-ext1[2]) &&
		(ext2[5]-ext2[4]) == (ext1[5]-ext1[4]))
	{
		return true;
	}
	std::cout << "image1 extent: " << cx::IntBoundingBox3D(ext1) << std::endl;
	std::cout << "image2 extent: " << cx::IntBoundingBox3D(ext2) << std::endl;
	return false;
}

bool RenderTester::hasValidDimensions(vtkImageDataPtr input)
{
	if(!input)
		return false;

	int dims[3];
	input->GetDimensions(dims);
	return (dims[0]*dims[1]*dims[2] > 0);
}


bool RenderTester::equalNumberOfComponents(vtkImageDataPtr image1, vtkImageDataPtr image2)
{
	return image1->GetNumberOfScalarComponents() == image2->GetNumberOfScalarComponents();
}


vtkImageDataPtr RenderTester::convertToColorImage(vtkImageDataPtr image)
{
	if (image->GetNumberOfScalarComponents() != 1)
		return image;

	vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
	for (unsigned i=0; i<3; ++i)
		merger->AddInputData(image);
	merger->Update();
	return merger->GetOutput();
}

bool RenderTester::findDifference(vtkImageDataPtr input1, vtkImageDataPtr input2)
{
	if (!this->equalNumberOfComponents(input1, input2))
	{
		std::cout << "converting input images to color..." << std::endl;
		input1 = this->convertToColorImage(input1);
		input2 = this->convertToColorImage(input2);
	}

//	std::cout << "=======================input1" << std::endl;
//	input1->Print(std::cout);
//	std::cout << "=======================input2" << std::endl;
//	input2->Print(std::cout);

	vtkImageDataPtr clipped1 = this->clipImage(input1);
	vtkImageDataPtr clipped2 = this->clipImage(input2);

	// Cannot compute difference unless image sizes are the same
	if (!this->equalExtent(clipped1, clipped2))
	{
		std::cout << "images have different sizes." << std::endl;
		return false;
	}

	vtkImageDifferencePtr imageDifference = vtkImageDifferencePtr::New();
	imageDifference->SetInputData(clipped1);
	imageDifference->SetImageData(clipped2);
	imageDifference->Update();
	double minError = imageDifference->GetThresholdedError();
	bool validdims = this->hasValidDimensions(imageDifference->GetOutput());
	if (!validdims)
	{
		std::cout << "image difference produced invalid image." << std::endl;
		return false;
	}

	bool success = minError < mImageErrorThreshold;

	if (!success)
	{
		std::cout << QString("Image difference=%1, threshold=%2").arg(minError).arg(mImageErrorThreshold) << std::endl;

		this->printFractionOfVoxelsAboveZero("Input1", input1);
		this->printFractionOfVoxelsAboveZero("Input2", input2);
		this->printFractionOfVoxelsAboveZero("Clipped1", clipped1);
		this->printFractionOfVoxelsAboveZero("Clipped2", clipped2);		
	}

	return success;
}

void RenderTester::printFractionOfVoxelsAboveZero(QString desc, vtkImageDataPtr image)
{
	double fraction = Utilities::getFractionOfVoxelsAboveThreshold(image, 0);
	std::cout << QString("%1 nonzero amount: %2").arg(desc).arg(fraction) << std::endl;

	QString path = cxtest::Utilities::getDataRoot("/"+desc+".png");
	this->writeToPNG(image, path);
	std::cout << QString("Save image to %1").arg(path) << std::endl;
}

void RenderTester::renderAndUpdateText(int num)
{
	for (int i = 0; i < num; ++i)
	{
		QString text(qstring_cast("Test: ")+qstring_cast(i));
		mMapper->SetInput(cstring_cast(text));
		mRenderWindow->Render();
	}
}

void RenderTester::addTextToVtkRenderWindow(QString text)
{
	mMapper = vtkTextMapperPtr::New();
	mMapper->SetInput(cstring_cast(text));
	mMapper->GetTextProperty()->SetColor(1, 0, 0);
	mMapper->GetTextProperty()->SetFontSize(10);
	mMapper->GetTextProperty()->SetFontFamilyToArial();

	vtkActor2DPtr actor = vtkActor2DPtr::New();
	actor->SetMapper(mMapper);

	REQUIRE(mRenderWindow->GetRenderers()->GetNumberOfItems()==1);

	mRenderWindow->GetRenderers()->GetFirstRenderer()->AddActor(actor);
}

} // namespace cxtest
