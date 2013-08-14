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

#include "sscVtkRenderTester.h"

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

#include "sscBoundingBox3D.h"
#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscDataReaderWriter.h"


typedef vtkSmartPointer<class vtkProp> vtkPropPtr;
typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;
typedef vtkSmartPointer<class vtkPNGWriter> vtkPNGWriterPtr;
typedef vtkSmartPointer<class vtkPNGReader> vtkPNGReaderPtr;

typedef vtkSmartPointer<class vtkImageDifference> vtkImageDifferencePtr;
typedef vtkSmartPointer<class vtkImageClip> vtkImageClipPtr;



namespace ssc
{


TestVtkRenderingPtr TestVtkRendering::create()
{
	return TestVtkRenderingPtr(new TestVtkRendering());
}

TestVtkRendering::TestVtkRendering()
{
//	mImageErrorThreshold = 1.0;
	mImageErrorThreshold = 100.0;
	mBorderOffset = 2;
	//		mView = new ssc::ViewWidget(NULL);
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

void TestVtkRendering::setImageErrorThreshold(double value)
{
    mImageErrorThreshold = value;
}

//	ssc::ViewWidget* getView()
//	{
//		return mView;
//	}

void TestVtkRendering::addProp(vtkPropPtr prop)
{
	mRenderer->AddViewProp(prop);
	//		this->getView()->getRenderer()->AddProp(prop);
}

void TestVtkRendering::renderToFile(QString filename)
{
	vtkImageDataPtr image = this->renderToImage();
	this->writeToPNG(image, filename);
}

void TestVtkRendering::alignRenderWindowWithImage(vtkImageDataPtr input)
{
	ssc::DoubleBoundingBox3D bounds(input->GetBounds());
	//		std::cout << "bounds " << bounds << std::endl;
	mRenderer->ResetCamera(bounds.data());

//	std::cout << "bounds " << bounds << std::endl;
//	std::cout << "spacing " << ssc::Vector3D(input->GetSpacing()) << std::endl;
//	std::cout << "extent " << ssc::IntBoundingBox3D(input->GetExtent()) << std::endl;
//	std::cout << "dim " << Eigen::Array3i(input->GetDimensions()) << std::endl;

	Eigen::Array3i dim(input->GetDimensions());
	mRenderWindow->SetSize(dim[0], dim[1]);

	// No idea why we must use bounds+1 here ... should be vp height in world space.
	// Set/Get the scaling used for a parallel projection, i.e. the height of the viewport in world-coordinate distances.
	//		std::cout << "(bounds.range()[1]) " << (bounds.range()[1]) << std::endl;
//	mRenderer->GetActiveCamera()->SetParallelScale((bounds.range()[1]+1)/2);
	mRenderer->GetActiveCamera()->SetParallelScale((bounds.range()[1])/2);
}

void TestVtkRendering::resetCamera()
{
	mRenderer->ResetCamera();
}

vtkImageDataPtr TestVtkRendering::renderToImage()
{
	//		ssc::DoubleBoundingBox3D bounds(mRenderer->ComputeVisiblePropBounds());
	//		std::cout << "bounds " << bounds << std::endl;
	//		mRenderer->ResetCamera(bounds.data());
	//		bounds[1] += 1;
	//		bounds[3] += 1;
	//		mRenderWindow->SetSize(bounds.range()[0], bounds.range()[1]);
	//		mRenderer->GetActiveCamera()->SetParallelScale((bounds.range()[1])/2);
	mRenderWindow->Render();

//		std::cout << "=======================input1" << std::endl;
//		mRenderer->GetActiveCamera()->Print(std::cout);

	//		this->getView()->getRenderer()->ResetCamera();
	//		this->getView()->getRenderWindow()->Render();

	return this->getImageFromRenderWindow();
}

vtkImageDataPtr TestVtkRendering::getImageFromRenderWindow()
{
	vtkWindowToImageFilterPtr windowToImageFilter = vtkWindowToImageFilterPtr::New();
	windowToImageFilter->SetInput(mRenderWindow);
	//		windowToImageFilter->SetInput(this->getView()->getRenderWindow());

	windowToImageFilter->Modified();
	windowToImageFilter->Update();

	return windowToImageFilter->GetOutput();
}

void TestVtkRendering::writeToPNG(vtkImageDataPtr image, QString filename)
{
	vtkPNGWriterPtr pngWriter = vtkPNGWriterPtr::New();
	pngWriter->SetFileName(filename.toStdString().c_str());
	pngWriter->SetInput(image);
	pngWriter->Write();
}

vtkImageDataPtr TestVtkRendering::readFromFile(QString filename)
{
	return DataReaderWriter().loadVtkImageData(filename);
//	if (QFileInfo(filename).)
//	vtkImageDataPtr load(const QString& filename);
}

vtkImageDataPtr TestVtkRendering::readFromPNG(QString filename)
{
	vtkPNGReaderPtr pngReader = vtkPNGReaderPtr::New();
	pngReader->SetFileName(filename.toStdString().c_str());
	pngReader->Update();
	return pngReader->GetOutput();
}

void TestVtkRendering::enterRunLoop()
{
	// Setup render window interactor
	vtkRenderWindowInteractorPtr renderWindowInteractor = vtkRenderWindowInteractorPtr::New();
	renderWindowInteractor->SetRenderWindow(mRenderWindow);
	// Render and start interaction
	mRenderWindow->Render();
	renderWindowInteractor->Start();
}

vtkImageDataPtr TestVtkRendering::clipImage(vtkImageDataPtr input)
{
	vtkImageClipPtr imageClip = vtkImageClipPtr::New();
	imageClip->SetClipData(1);
	imageClip->SetInput(input);

	int* wExt1 = imageClip->GetInput()->GetWholeExtent();
	imageClip->SetOutputWholeExtent(wExt1[0] + this->mBorderOffset,
									wExt1[1] - this->mBorderOffset,
									wExt1[2] + this->mBorderOffset,
									wExt1[3] - this->mBorderOffset,
									wExt1[4],
									wExt1[5]);
	imageClip->Update();
	return imageClip->GetOutput();
}

bool TestVtkRendering::equalExtent(vtkImageDataPtr input1, vtkImageDataPtr input2)
{
	int ext1[6], ext2[6];
	input1->Update();
	input1->GetExtent(ext1);
	input2->Update();
	input2->GetExtent(ext2);

	if ((ext2[1]-ext2[0]) == (ext1[1]-ext1[0]) &&
		(ext2[3]-ext2[2]) == (ext1[3]-ext1[2]) &&
		(ext2[5]-ext2[4]) == (ext1[5]-ext1[4]))
	{
		return true;
	}
	std::cout << "image1 extent: " << ssc::IntBoundingBox3D(ext1) << std::endl;
	std::cout << "image2 extent: " << ssc::IntBoundingBox3D(ext2) << std::endl;
	return false;
}

bool TestVtkRendering::hasValidDimensions(vtkImageDataPtr input)
{
	if(!input)
		return false;

	int dims[3];
	input->GetDimensions(dims);
	return (dims[0]*dims[1]*dims[2] > 0);
}


bool TestVtkRendering::equalNumberOfComponents(vtkImageDataPtr image1, vtkImageDataPtr image2)
{
	return image1->GetNumberOfScalarComponents() == image2->GetNumberOfScalarComponents();
}


vtkImageDataPtr TestVtkRendering::convertToColorImage(vtkImageDataPtr image)
{
	if (image->GetNumberOfScalarComponents() != 1)
		return image;

	vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
	for (unsigned i=0; i<3; ++i)
		merger->SetInput(i, image);
	merger->Update();
	return merger->GetOutput();
}

bool TestVtkRendering::findDifference(vtkImageDataPtr input1, vtkImageDataPtr input2)
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
	imageDifference->SetInput(clipped1);
	imageDifference->SetImage(clipped2);
	imageDifference->Update();
	double minError = imageDifference->GetThresholdedError();
	bool validdims = this->hasValidDimensions(imageDifference->GetOutput());
	if (!validdims)
	{
		std::cout << "image difference produced invalid image." << std::endl;
		return false;
	}

	std::cout << QString("Image difference=%1, threshold=%2").arg(minError).arg(mImageErrorThreshold) << std::endl;
	return minError < mImageErrorThreshold;
}

} // namespace ssc
