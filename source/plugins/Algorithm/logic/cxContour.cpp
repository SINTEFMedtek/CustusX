#include "cxContour.h"

#include <vtkImageShrink3D.h>
#include <vtkMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>

#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMesh.h"
#include "sscRegistrationTransform.h"
#include "sscTypeConversions.h"

namespace cx
{
Contour::Contour() :
    ThreadedTimedAlgorithm<vtkPolyDataPtr>("surface", 20)
{
	mUseDefaultMessages = false;
}

Contour::~Contour()
{}

void Contour::setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, double decimation,
		bool reduceResolution, bool smoothing, bool preserveTopology)
{
	mInput = image;
	mOutputBasePath = outputBasePath;
	mThreshold = threshold;
	mDecimation = decimation;
	mUseReduceResolution = reduceResolution;
	mUseSmoothing = smoothing;
	mPreserveTopology = preserveTopology;

//	this->generate();
}

ssc::MeshPtr Contour::getOutput()
{
  return mOutput;
}

void Contour::postProcessingSlot()
{
  vtkPolyDataPtr cubesPolyData = this->getResult();
  if(!cubesPolyData)
  {
    ssc::messageManager()->sendError("Centerline extraction failed.");
    return;
  }

  QString uid = mInput->getUid() + "_ge%1";
  QString name = mInput->getName() + " ge%1";
  mOutput = ssc::dataManager()->createMesh(cubesPolyData, uid, name, "Images");

  mOutput->get_rMd_History()->setRegistration(mInput->get_rMd());
  mOutput->get_rMd_History()->setParentSpace(mInput->getUid());

  ssc::dataManager()->loadData(mOutput);
  ssc::dataManager()->saveMesh(mOutput, mOutputBasePath);

  ssc::messageManager()->sendSuccess(QString("Created contour \"%1\" [%2s]").arg(mOutput->getName()).arg(this->getSecondsPassedAsString()));

//  emit finished();
}

vtkPolyDataPtr Contour::calculate()
{
	ssc::messageManager()->sendInfo(QString("Creating contour from \"%1\"...").arg(mInput->getName()));

  //Shrink input volume
  vtkImageShrink3DPtr shrinker = vtkImageShrink3DPtr::New();
  if(mUseReduceResolution)
  {
    shrinker->SetInput(mInput->getBaseVtkImageData());
    shrinker->SetShrinkFactors(2,2,2);
    shrinker->Update();
  }

  // Find countour
  vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
  if(mUseReduceResolution)
    convert->SetInput(shrinker->GetOutput());
  else
    convert->SetInput(mInput->getBaseVtkImageData());

  std::cout << "convert old  in: " << convert->GetInput() << " t=" << mThreshold << std::endl;

  convert->SetValue(0, mThreshold);
  convert->Update();

  vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
  cubesPolyData = convert->GetOutput();

  // Smooth surface model
  vtkWindowedSincPolyDataFilterPtr smoother = vtkWindowedSincPolyDataFilterPtr::New();
  if(mUseSmoothing)
  {
    smoother->SetInput(cubesPolyData);
    smoother->Update();
    cubesPolyData = smoother->GetOutput();
  }

  //Create a surface of triangles

  //Decimate surface model (remove a percentage of the polygons)
  vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
  vtkDecimateProPtr deci = vtkDecimateProPtr::New();
  vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
  if (mDecimation > 0.000001)
  {
    trifilt->SetInput(cubesPolyData);
    trifilt->Update();
    deci->SetInput(trifilt->GetOutput());
    deci->SetTargetReduction(mDecimation);
    deci->SetPreserveTopology(mPreserveTopology);
//    deci->PreserveTopologyOn();
    deci->Update();
    cubesPolyData = deci->GetOutput();
  }

  normals->SetInput(cubesPolyData);
  normals->Update();

  cubesPolyData->DeepCopy(normals->GetOutput());

  return cubesPolyData;
}

}//namespace cx
