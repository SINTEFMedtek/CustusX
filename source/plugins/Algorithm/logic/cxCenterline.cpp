#include "cxCenterline.h"

#include "cxAlgorithmHelpers.h"

#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "sscUtilHelpers.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "sscMesh.h"
#include "cxPatientService.h"

namespace cx
{
Centerline::Centerline() :
    ThreadedTimedAlgorithm<vtkImageDataPtr>("centerline", 20),
    mDefaultColor("red")
{
	mUseDefaultMessages = false;
}

Centerline::~Centerline()
{}

void Centerline::setDefaultColor(QColor color)
{
  mDefaultColor = color;
}

bool Centerline::setInput(ssc::ImagePtr inputImage, QString outputBasePath)
{
  mInput = inputImage;
  mOutputBasePath = outputBasePath;

  if (!mInput)
  {
    return false;
  }
  if (mInput->getMax() != 1 || mInput->getMin() != 0)
  {
    ssc::messageManager()->sendError("Centerline algorithm requires binary volume as input");
    return false;
  }

  return true;
}

ssc::DataPtr Centerline::getOutput()
{
  return mOutput;
}

void Centerline::postProcessingSlot()
{
  vtkImageDataPtr rawResult = this->getResult();
  if(!rawResult)
  {
    ssc::messageManager()->sendError("Centerline extraction failed.");
    return;
  }

  ssc::ImagePtr outImage = ssc::dataManager()->createDerivedImage(rawResult,mInput->getUid() + "_cl_temp%1", mInput->getName()+" cl_temp%1", mInput);
  outImage->resetTransferFunctions();

  //automatically generate a mesh from the centerline
  vtkPolyDataPtr centerlinePolyData = ssc::SeansVesselReg::extractPolyData(outImage, 1, 0);

  QString uid = mInput->getUid() + "_cl%1";
  QString name = mInput->getName()+" cl%1";
  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
  mesh->setColor(mDefaultColor);
  mesh->get_rMd_History()->setParentSpace(mInput->getUid());
  ssc::dataManager()->loadData(mesh);
  ssc::dataManager()->saveMesh(mesh, mOutputBasePath);
  mOutput = mesh;

  ssc::messageManager()->sendSuccess(QString("Created centerline \"%1\" [%2s]").arg(mOutput->getName()).arg(this->getSecondsPassedAsString()));

  emit finished();
}


vtkImageDataPtr Centerline::calculate()
{
    // Centerline algorithm requires binary volume as input
	  if (!mInput)
	  {
	    return vtkImageDataPtr();
	  }
	  if (mInput->getMax() != 1 || mInput->getMin() != 0)
	  {
	    return vtkImageDataPtr();
	  }

	  ssc::messageManager()->sendInfo(QString("Creating centerline from \"%1\"...").arg(mInput->getName()));

  itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(mInput);

  //Centerline extraction
  typedef itk::BinaryThinningImageFilter3D<itkImageType, itkImageType> centerlineFilterType;
  centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
  centerlineFilter->SetInput(itkImage);
  centerlineFilter->Update();
  itkImage = centerlineFilter->GetOutput();

  //Convert ITK to VTK
  itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
  itkToVtkFilter->SetInput(itkImage);
  itkToVtkFilter->Update();

  vtkImageDataPtr rawResult = vtkImageDataPtr::New();
  rawResult->DeepCopy(itkToVtkFilter->GetOutput());

  return rawResult;
}
}//namespace cx
