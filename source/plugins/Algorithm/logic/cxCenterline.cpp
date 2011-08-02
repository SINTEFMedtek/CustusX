#include "cxCenterline.h"

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
    ThreadedTimedAlgorithm<vtkImageDataPtr>("centerline", 10),
    mDefaultColor("red")
{}

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

  if (mInput->getMax() != 1 || mInput->getMin() != 0)
  {
    ssc::messageManager()->sendError("Centerline algorithm requires binary volume as input");
    return false;
  }

  this->generate();
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

  ssc::ImagePtr outImage = ssc::dataManager()->createImage(rawResult,mInput->getUid() + "_cl_temp%1", mInput->getName()+" cl_temp%1");

  outImage->get_rMd_History()->setRegistration(mInput->get_rMd());
  outImage->get_rMd_History()->setParentSpace(mInput->getUid());

  //automatically generate a mesh from the centerline
  vtkPolyDataPtr centerlinePolyData = SeansVesselReg::extractPolyData(outImage, 1, 0);

  QString uid = mInput->getUid() + "_cl%1";
  QString name = mInput->getName()+" cl%1";
  ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
  mesh->setColor(mDefaultColor);
  mesh->get_rMd_History()->setParentSpace(mInput->getUid());
  ssc::dataManager()->loadData(mesh);
  ssc::dataManager()->saveMesh(mesh, mOutputBasePath);
  mOutput = mesh;

  ssc::messageManager()->sendSuccess("Created centerline \"" + mOutput->getName()+"\"");

  emit finished();
}

vtkImageDataPtr Centerline::calculate()
{
  ssc::messageManager()->sendInfo("Generating \""+mInput->getName()+"\" centerline... Please wait!");

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
