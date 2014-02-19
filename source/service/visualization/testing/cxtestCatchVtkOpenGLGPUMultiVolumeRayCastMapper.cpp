
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
#include "sscLogger.h"

//#include <QString>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
#include <string>
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"

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
typedef vtkSmartPointer<vtkVolumePicker> vtkMultiVolumePickerPtr;
//typedef vtkSmartPointer<vtkMultiVolumePicker> vtkMultiVolumePickerPtr;
#include "vtkForwardDeclarations.h"

typedef vtkSmartPointer<vtkOpenGLGPUMultiVolumeRayCastMapper> vtkOpenGLGPUMultiVolumeRayCastMapperPtr;

namespace cxtest
{

class VtkOpenGLGPUMultiVolumeRayCastMapperFixture
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
		mVolume->SetProperty(property);
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

		iren->Start();
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
		SSC_LOG("A");
		this->renderOnce();
		vtkRendererPtr renderer = this->getRenderer();
		vtkCameraPtr camera = renderer->GetActiveCamera();
		Eigen::Vector3d p_camera = pos;
		Eigen::Vector3d p_focus = p_camera + dir;
		camera->SetPosition(p_camera.data());
		camera->SetFocalPoint(p_focus.data());
		renderer->ResetCameraClippingRange();
		std::cout << "p_camera " << p_camera << std::endl;
		std::cout << "p_focus " << p_focus << std::endl;
		this->renderOnce();

		vtkMultiVolumePickerPtr picker = vtkMultiVolumePickerPtr::New();
//		Eigen::Vector3d p_click = p_camera;
		Eigen::Vector3d p_click(100,100,0);
		int hit = picker->Pick(p_click[0], p_click[1], 0, renderer);
		std::cout << "p_click " << p_click << std::endl;
		std::cout << "hit " << hit << std::endl;

		if (hit)
			return picker;
		else
			return vtkMultiVolumePickerPtr();
	}

private:
	vtkVolumePtr mVolume;
	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mMapper;

	vtkRenderWindowPtr mRenderWindow;
	vtkRendererPtr mRenderer;
};


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
	vtkVolumePropertyPtr property = fixture.getVolumeProperty(100,300);
	Eigen::Array3i imageDim(image->GetDimensions());
	Eigen::Array3d imageSpacing(image->GetSpacing());

	int shift = imageDim[0]/2;

	vtkImageDataPtr refImage = fixture.createVtkImageData(imageDim+Eigen::Array3i(shift,0,0), imageSpacing, 0);

	fixture.setReferenceImage(refImage, fixture.getVolumeProperty(100,300));
	fixture.addImage(0, image, property);
	fixture.addImage(1, image, property, Eigen::Vector3d(imageSpacing[0]*shift,0,0));

	fixture.renderLoop();

	REQUIRE(1); // makes this a valid catch test
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can render 3 small volumes without crashing", "[unit][gui]")
{
	int numberOfVolumes = 3;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = fixture.createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 0);
	vtkVolumePropertyPtr property = fixture.getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	for (unsigned i=0; i<numberOfVolumes; ++i)
	{
		vtkImageDataPtr image = fixture.createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 200);
		fixture.addImage(i, image, property);
	}

	fixture.renderOnce();

	REQUIRE(1); // makes this a valid catch test
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can render 1 small volume without crashing", "[unit][gui]")
{
	int numberOfVolumes = 1;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	vtkImageDataPtr refImage = fixture.createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 0);
	vtkImageDataPtr image = fixture.createVtkImageData(Eigen::Array3i(5,5,5), Eigen::Array3d(1,1,1), 200);
	vtkVolumePropertyPtr property = fixture.getVolumeProperty(100,300);

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
	vtkImageDataPtr refImage = fixture.createVtkImageData(Eigen::Array3i(51,51,51), Eigen::Array3d(2,2,2), 200);
	vtkVolumePropertyPtr property = fixture.getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	// volume extent=(0..10, 0..10, 0..10)
	fixture.addImage(0, refImage, property);

	Eigen::Vector3d p_camera(55,55,-100);
	Eigen::Vector3d viewdir = Eigen::Vector3d(0,0,1);

	vtkMultiVolumePickerPtr picker = fixture.pickAlongRayReturnSuccessfulMultiVolumePicker(p_camera, viewdir);

	Eigen::Vector3d p_expected(55,55,0);
	REQUIRE(picker.GetPointer());

	//vtkDataSetPtr data = picker->GetDataSet();
	Eigen::Vector3d p_pick(picker->GetPickPosition());
	REQUIRE(p_pick.isApprox(p_expected));

	REQUIRE(1); // makes this a valid catch test
}

TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can be picked using vtkMultiVolumePicker", "[unit][hide]")
{
	int numberOfVolumes = 3;
	VtkOpenGLGPUMultiVolumeRayCastMapperFixture fixture(numberOfVolumes);

	// volume extent=(0..100, 0..100, 0..100)
	vtkImageDataPtr refImage = fixture.createVtkImageData(Eigen::Array3i(51,51,51), Eigen::Array3d(2,2,2), 0);
	vtkVolumePropertyPtr property = fixture.getVolumeProperty(100,300);

	fixture.setReferenceImage(refImage, property);

	// volume extent=(0..10, 0..10, 0..10)
	vtkImageDataPtr image = fixture.createVtkImageData(Eigen::Array3i(11,11,11), Eigen::Array3d(1,1,1), 200);
	fixture.addImage(0, image, property, Eigen::Array3d(0,0,0));
	fixture.addImage(1, image, property, Eigen::Array3d(50,0,0));
	fixture.addImage(2, image, property, Eigen::Array3d(50,50,0));

	Eigen::Vector3d p_camera(55,55,-100);
	Eigen::Vector3d viewdir = Eigen::Vector3d(0,0,1);

	vtkMultiVolumePickerPtr picker = fixture.pickAlongRayReturnSuccessfulMultiVolumePicker(p_camera, viewdir);

	Eigen::Vector3d p_expected(55,55,0);
	REQUIRE(picker.GetPointer());

	//vtkDataSetPtr data = picker->GetDataSet();
	Eigen::Vector3d p_pick(picker->GetPickPosition());
	REQUIRE(p_pick.isApprox(p_expected));

	REQUIRE(1); // makes this a valid catch test
}

} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
