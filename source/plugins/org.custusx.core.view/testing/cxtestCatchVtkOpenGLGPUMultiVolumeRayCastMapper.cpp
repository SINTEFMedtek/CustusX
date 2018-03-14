/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "catch.hpp"
#include <vtkTransform.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkImageData.h>
#include "vtkMetaImageReader.h"
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkMultiVolumePicker.h"
#include <vtkCamera.h>


#include "cxVector3D.h" //Neeed to setup Eigen correctly

#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include "vtkGPUVolumeRayCastMapper.h"

#include "cxtestRenderTester.h"
#include "cxtestUtilities.h"

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<vtkRenderWindow> vtkRenderWindowPtr;
typedef vtkSmartPointer<vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef vtkSmartPointer<vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<vtkVolume> vtkVolumePtr;
typedef vtkSmartPointer<vtkTransform> vtkTransformPtr;
typedef vtkSmartPointer<vtkMetaImageReader> vtkMetaImageReaderPtr;
typedef vtkSmartPointer<vtkRenderWindowInteractor> vtkRenderWindowInteractorPtr;
typedef vtkSmartPointer<vtkInteractorStyleTrackballCamera> vtkInteractorStyleTrackballCameraPtr;
typedef vtkSmartPointer<vtkMultiVolumePicker> vtkMultiVolumePickerPtr;
#include "vtkForwardDeclarations.h"

typedef vtkSmartPointer<vtkOpenGLGPUMultiVolumeRayCastMapper> vtkOpenGLGPUMultiVolumeRayCastMapperPtr;
typedef vtkSmartPointer<vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;

namespace cxtest
{

class vtkMapperFixture
{
public:
	void requireRender()
	{
		vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
		vtkRendererPtr renderer = vtkRendererPtr::New();
		renderWindow->AddRenderer(renderer);
		renderWindow->SetSize(30,30);
		renderer->AddVolume(mVolume);
		renderWindow->Render();
		renderWindow->Render();

		RenderTesterPtr renderTester = cxtest::RenderTester::create(renderWindow);
		vtkImageDataPtr output = renderTester->renderToImage();
		double fraction = cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, 0);
	//	std::cout << "fraction: " << fraction << std::endl;
		REQUIRE(fraction > 0);
		REQUIRE(fraction < 1);
	}

	void renderOnce()
	{
		if (!mRenderWindow)
		{
			mRenderWindow = vtkRenderWindowPtr::New();
			mRenderer = vtkRendererPtr::New();
			mRenderWindow->AddRenderer(mRenderer);
			mRenderWindow->SetSize(200,200);
			mRenderer->AddVolume(mVolume);
		}
		mRenderWindow->Render();
	}
	vtkRendererPtr getRenderer() { return mRenderer; }

	vtkImageDataPtr loadVtkImageData(std::string filename)
	{
		vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
		reader->SetFileName(filename.c_str());
		reader->ReleaseDataFlagOn();
		reader->Update();
		return reader->GetOutput();
	}


	vtkMultiVolumePickerPtr pickAlongRayReturnSuccessfulMultiVolumePicker(Eigen::Vector3d pos, Eigen::Vector3d dir)
	{
		this->renderOnce();
		vtkRendererPtr renderer = this->getRenderer();
		vtkCameraPtr camera = renderer->GetActiveCamera();
		Eigen::Vector3d p_camera = pos;
		Eigen::Vector3d p_focus = p_camera + dir;
		camera->SetPosition(p_camera.data());
		camera->SetFocalPoint(p_focus.data());
		renderer->ResetCameraClippingRange();
//		std::cout << "p_camera " << p_camera << std::endl;
//		std::cout << "p_focus " << p_focus << std::endl;
//		this->renderOnce();

		vtkMultiVolumePickerPtr picker = vtkMultiVolumePickerPtr::New();
//		Eigen::Vector3d p_click = p_camera;
		Eigen::Vector3d p_click(100,100,0);//In the middle of mRenderWindow size
		int hit = picker->Pick(p_click[0], p_click[1], 0, renderer);
//		std::cout << "p_click " << p_click << std::endl;
//		std::cout << "hit " << hit << std::endl;

		if (hit)
			return picker;
		else
			return vtkMultiVolumePickerPtr();
	}

	void requireHit(int x, int y, bool ignoreExpected = false)
	{
		Eigen::Vector3d p_camera(x,y,-100);
		Eigen::Vector3d viewdir = Eigen::Vector3d(0,0,1);
		vtkMultiVolumePickerPtr picker = pickAlongRayReturnSuccessfulMultiVolumePicker(p_camera, viewdir);
		Eigen::Vector3d p_expected(x,y,0);
		{
			INFO("Should hit point: " << x << " " << y);
			REQUIRE(picker.GetPointer());
		}
		Eigen::Vector3d p_pick(picker->GetPickPosition());
		if(!ignoreExpected)
		{
			INFO(p_pick << " == " << p_expected);
			CHECK(p_pick.isApprox(p_expected));
		}
	}

	void requireMiss(int x, int y)
	{
		Eigen::Vector3d p_camera(x,y,-100);
		Eigen::Vector3d viewdir = Eigen::Vector3d(0,0,1);
		vtkMultiVolumePickerPtr picker = pickAlongRayReturnSuccessfulMultiVolumePicker(p_camera, viewdir);

		INFO("Should not hit point: " << x << " " << y);
		REQUIRE_FALSE(picker.GetPointer());
	}

protected:
	vtkVolumePtr mVolume;

	vtkRenderWindowPtr mRenderWindow;
	vtkRendererPtr mRenderer;
};

class vtkGPUVolumeRayCastMapperFixture : public vtkMapperFixture
{
public:
	explicit vtkGPUVolumeRayCastMapperFixture()
	{
		mMapper = vtkGPUVolumeRayCastMapperPtr::New();
		mVolume = vtkVolumePtr::New();
		mVolume->SetMapper(mMapper);
	}

	void addImage(vtkImageDataPtr image, Eigen::Vector3d translation = Eigen::Vector3d::Zero())
	{
		vtkTransformPtr transform = vtkTransformPtr::New();
		transform->Identity();
		transform->Translate(translation.data());
		transform->Update();

		mVolume->SetUserTransform(transform);
		mMapper->SetInput(image);
	}

private:
	vtkGPUVolumeRayCastMapperPtr mMapper;

};

class VtkOpenGLGPUMultiVolumeRayCastMapperFixture : public vtkMapperFixture
{
public:
	explicit VtkOpenGLGPUMultiVolumeRayCastMapperFixture(int volumeCount)
	{
		mMapper = vtkOpenGLGPUMultiVolumeRayCastMapperPtr::New();
		mMapper->setNumberOfAdditionalVolumes(volumeCount);
		mVolume = vtkVolumePtr::New();
		mVolume->SetMapper(mMapper);
	}

	void setReferenceImage(vtkImageDataPtr image, vtkVolumePropertyPtr property)
	{
		mMapper->SetInput(0, image);

		vtkTransformPtr transform = vtkTransformPtr::New();
		transform->Identity();

		mVolume->SetProperty(property);

		Eigen::Vector3d translation = Eigen::Array3d(0,0,0);
		transform->Translate(translation.data());
		transform->Update();

		mVolume->SetUserMatrix(transform->GetMatrix()); //set rMd
	}

	void addImage(int index, vtkImageDataPtr image, vtkVolumePropertyPtr property, Eigen::Vector3d translation = Eigen::Vector3d::Zero())
	{
		int currentIndex = index; // index into additional volumes.

		vtkTransformPtr transform = vtkTransformPtr::New();
		transform->Identity();
		transform->Translate(translation.data());
		transform->Update();

		mMapper->SetAdditionalInputUserTransform(currentIndex,transform);
		mMapper->SetInput(currentIndex+1, image);
		mMapper->SetAdditionalProperty(currentIndex, property);
	}

	void addImageAndRotate(int index, vtkImageDataPtr image, vtkVolumePropertyPtr property, Eigen::Vector3d translation = Eigen::Vector3d::Zero())
	{
		int currentIndex = index; // index into additional volumes.

		vtkTransformPtr transform = vtkTransformPtr::New();
		transform->Identity();
		transform->RotateX(30);
		transform->Translate(translation.data());
		transform->Update();

		mMapper->SetAdditionalInputUserTransform(currentIndex,transform);
		mMapper->SetInput(currentIndex+1, image);
		mMapper->SetAdditionalProperty(currentIndex, property);
	}

	void renderLoop()
	{
		vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
		vtkRendererPtr renderer = vtkRendererPtr::New();
		renderWindow->AddRenderer(renderer);

		vtkRenderWindowInteractorPtr iren = vtkRenderWindowInteractorPtr::New();
		iren->SetRenderWindow(renderWindow);
		iren->GetInteractorStyle()->SetDefaultRenderer(renderer);

		vtkInteractorStyleTrackballCameraPtr interact = vtkInteractorStyleTrackballCameraPtr::New();
		iren->SetInteractorStyle(interact);

		renderWindow->SetSize(600,600);
		renderer->AddVolume(mVolume);

		renderWindow->Render();
		iren->Initialize();
		iren->Start();
	}

private:
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mMapper;
};

namespace {

vtkVolumePropertyPtr getVolumeProperty(int minVal, int maxVal)
{
	vtkVolumePropertyPtr retval = vtkVolumePropertyPtr::New();
	vtkPiecewiseFunctionPtr opacityTF = vtkPiecewiseFunctionPtr::New();
	vtkColorTransferFunctionPtr colorTF = vtkColorTransferFunctionPtr::New();

	opacityTF->AddPoint(minVal, 0.0);
	opacityTF->AddPoint(maxVal, 1.0);

	colorTF->SetColorSpaceToRGB();
	colorTF->AddRGBPoint(minVal, 0.0, 0.0, 0.0);
	colorTF->AddRGBPoint(maxVal, 1.0, 1.0, 1.0);

	retval->SetColor(colorTF);
	retval->SetScalarOpacity(opacityTF);
	retval->SetInterpolationTypeToLinear();
	return retval;
}

vtkImageDataPtr createVtkImageData(Eigen::Array3i dim,
									 Eigen::Array3d spacing,
									 unsigned short initValue)
{
	vtkImageDataPtr data = vtkImageDataPtr::New();
	data->SetSpacing(spacing[0], spacing[1], spacing[2]);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
	data->SetScalarType(VTK_UNSIGNED_SHORT);
	data->SetNumberOfScalarComponents(1);
	data->AllocateScalars();

	int scalarSize = dim[0]*dim[1]*dim[2];

	unsigned short* ptr = reinterpret_cast<unsigned short*>(data->GetScalarPointer());
	std::fill(ptr, ptr+scalarSize, initValue);

	// A trick to get a full LUT in Image (automatic LUT generation)
	// Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
	if (scalarSize > 0)
	{
		ptr[0] = 255;
		if (scalarSize > 1)
			ptr[1] = 0;
		data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
		ptr[0] = initValue;
		if (scalarSize > 1)
			ptr[1] = initValue;
	}
	data->UpdateInformation(); // update extent etc

	return data;
}

} //Empty namespace

//-----

/** Test that tests vtkOpenGLGPUMultiVolumeRayCastMapper directly, without using MehdiGPURayCastMultiVolumeRep
  *
  * Use this manual test to debug the class. Visual feedback only.
  */
TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can two kaisa in separate locations", "[manual][gui][ca_mul3d]")
{
	int numberOfVolumes = 2;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	std::string filename = "/home/christiana/dev/working/CustusX3/CustusX3/data/Phantoms/Kaisa/MetaImage/Kaisa.mhd";

	vtkImageDataPtr image = fixture.loadVtkImageData(filename);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);
	Eigen::Array3i imageDim(image->GetDimensions());
	Eigen::Array3d imageSpacing(image->GetSpacing());

	int shift = imageDim[0]/2;

	vtkImageDataPtr refImage = createVtkImageData(imageDim+Eigen::Array3i(shift,0,0), imageSpacing, 0);

	fixture.setReferenceImage(refImage, getVolumeProperty(100,300));
	fixture.addImage(0, image, property);
	fixture.addImage(1, image, property, Eigen::Vector3d(imageSpacing[0]*shift,0,0));

	fixture.renderLoop();

	REQUIRE(1); // makes this a valid catch test
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can render 3 small volumes without crashing", "[unit][gui]")
{
	int numberOfVolumes = 3;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 0);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	for (unsigned i=0; i<numberOfVolumes; ++i)
	{
		vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 200);
		fixture.addImage(i, image, property);
	}

	fixture.renderOnce();

	REQUIRE(1); // makes this a valid catch test
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can render 1 small volume without crashing", "[unit][gui]")
{
	int numberOfVolumes = 1;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 0);
	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 200);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);
	fixture.addImage(0, image, property);

	fixture.renderOnce();

	REQUIRE(1); // makes this a valid catch test
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper vs vtkMultiVolumePicker: Simple test", "[unit]")
{
	int numberOfVolumes = 1;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	// volume extent=(0..100, 0..100, 0..100)
	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(101,101,101), Eigen::Array3d(2,2,2), 200);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	// volume extent=(0..10, 0..10, 0..10)
	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(101,101,101), Eigen::Array3d(1,1,1), 200);
	fixture.addImage(0, image, property, Eigen::Array3d(0,0,0));

	Eigen::Vector3d p_camera(55,55,-100);
	Eigen::Vector3d viewdir = Eigen::Vector3d(0,0,1);

	vtkMultiVolumePickerPtr picker = fixture.pickAlongRayReturnSuccessfulMultiVolumePicker(p_camera, viewdir);

	Eigen::Vector3d p_expected(55,55,0);
	REQUIRE(picker.GetPointer());

	Eigen::Vector3d p_pick(picker->GetPickPosition());
	{
		INFO(p_pick << " == " << p_expected);
		REQUIRE(p_pick.isApprox(p_expected));
	}
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper vs vtkMultiVolumePicker: Origin", "[unit]")
{
	int numberOfVolumes = 1;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(21,21,21), Eigen::Array3d(1,1,1), 200);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(11,11,11), Eigen::Array3d(1,1,1), 200);
	fixture.addImage(0, image, property, Eigen::Array3d(0,0,0));

	fixture.requireRender();
	fixture.requireHit(0, 0);
	fixture.requireHit(10, 10);
	fixture.requireMiss(11, 11);
	fixture.requireMiss(20, 20);
	fixture.requireRender();
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper vs vtkMultiVolumePicker: Small target", "[unit]")
{
	int numberOfVolumes = 1;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(21,21,21), Eigen::Array3d(1,1,1), 100);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(11,11,11), Eigen::Array3d(1,1,1), 300);
	fixture.addImage(0, image, property, Eigen::Array3d(10,10,0));

	fixture.requireRender();
	fixture.requireMiss(0, 0);
	fixture.requireMiss(9, 9);
	fixture.requireHit(10, 10);
	fixture.requireHit(20, 20);

	fixture.requireMiss(9, 10);
	fixture.requireMiss(9, 20);
	fixture.requireMiss(10, 9);
	fixture.requireMiss(10, 21);
	fixture.requireMiss(21, 10);
	fixture.requireMiss(21, 20);
	fixture.requireMiss(20, 21);
	fixture.requireRender();
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper vs vtkMultiVolumePicker: Rotation", "[unit]")
{
	int numberOfVolumes = 1;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(21,21,21), Eigen::Array3d(1,1,1), 100);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(11,11,11), Eigen::Array3d(1,1,1), 300);
	fixture.addImageAndRotate(0, image, property, Eigen::Array3d(10,10,0));

	fixture.requireRender();
	fixture.requireMiss(0, 0);
	fixture.requireMiss(9, 9);
	fixture.requireHit(10, 10, true);
	fixture.requireMiss(20, 20);

	fixture.requireMiss(9, 10);
	fixture.requireMiss(9, 20);
	fixture.requireHit(10, 9, true);
	fixture.requireMiss(10, 21);
	fixture.requireMiss(21, 10);
	fixture.requireMiss(21, 20);
	fixture.requireMiss(20, 21);
	fixture.requireRender();
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can be picked using vtkMultiVolumePicker", "[unit]")
{
	int numberOfVolumes = 3;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	// volume extent=(0..40, 0..40, 0..40)
	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(41,41,41), Eigen::Array3d(1,1,1), 0);
	vtkVolumePropertyPtr property = getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	// volume extent=(0..10, 0..10, 0..10)
	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(11,11,11), Eigen::Array3d(1,1,1), 200);
	fixture.addImage(0, image, property, Eigen::Array3d(0,0,0));
	fixture.addImage(1, image, property, Eigen::Array3d(20,0,0));
	fixture.addImageAndRotate(2, image, property, Eigen::Array3d(20,20,0));
//	fixture.addImage(2, image, property, Eigen::Array3d(20,20,0));

	fixture.requireRender();
	fixture.requireHit(5, 5);
	fixture.requireMiss(15, 0);
	fixture.requireMiss(15, 15);
	fixture.requireMiss(0, 15);
	fixture.requireHit(25, 5);
	fixture.requireHit(25, 20, true);
	fixture.requireMiss(40, 0);
	fixture.requireMiss(40, 40);
	fixture.requireRender();
}

TEST_CASE("vtkTransform translate and rotate", "[unit]")
{
	vtkTransformPtr transform = vtkTransformPtr::New();
	transform->Identity();
	transform->RotateX(60);
	Eigen::Array3d translation = Eigen::Array3d(0,10,20);
	transform->Translate(translation.data());
	transform->Update();


	double p1[3] = {10, 20, 30};
	double p1_t[3];

	transform->PreMultiply();
	transform->TransformPoint(p1, p1_t);
	CHECK(p1_t[0] == p1[0]);
	CHECK(p1_t[1] != p1[1]);
	CHECK(p1_t[2] != p1[2]);
}

TEST_CASE("vtkGPUVolumeRayCastMapper vs vtkMultiVolumePicker: Small target", "[unit]")
{
	vtkGPUVolumeRayCastMapperFixture fixture;

	vtkImageDataPtr refImage = createVtkImageData(Eigen::Array3i(21,21,21), Eigen::Array3d(1,1,1), 100);

	vtkImageDataPtr image = createVtkImageData(Eigen::Array3i(11,11,11), Eigen::Array3d(1,1,1), 300);
	fixture.addImage(image, Eigen::Array3d(10,10,0));

	fixture.requireRender();
	fixture.requireMiss(0, 0);
	fixture.requireMiss(9, 9);
	fixture.requireHit(10, 10);
	fixture.requireHit(20, 20);

	fixture.requireMiss(9, 10);
	fixture.requireMiss(9, 20);
	fixture.requireMiss(10, 9);
	fixture.requireMiss(10, 21);
	fixture.requireMiss(21, 10);
	fixture.requireMiss(21, 20);
	fixture.requireMiss(20, 21);
	fixture.requireRender();
}


} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
