
#include "cxShiftCorrectionWidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QDir>
#include <QStringList>
#include <QLineEdit>
#include <QSpinBox>
#include <vtkImageThreshold.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>

#include <vtkMetaImageReader.h>
#include <vtkMatrix4x4.h>
#include <vtkMINCImageWriter.h>
#include <vtkMetaImageWriter.h>

//ITK
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryThinningImageFilter.h>

#include <vtkImageToPolyDataFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkMarchingCubes.h>
#include <vtkProperty.h>

#include "sscMessageManager.h"
#include "sscMesh.h"
#include "sscGeometricRep.h"
#include "cxDataManager.h"
#include "cxRepManager.h"
#include "cxViewManager.h"
#include "cxView3D.h"

//test
#include <vtkConeSource.h>

namespace cx
{
ShiftCorrectionWidget::ShiftCorrectionWidget(QWidget* parent) :
  QWidget(parent),
  //mLayout(new QVBoxLayout(this)),
  mLayout(new QGridLayout(this)),
  mMovingImageSegmentButton(new QPushButton("Vessel segmentation")),
  mRefImageSegmentButton(new QPushButton("Vessel segmentation")),
  mMovingImageProcessButton(new QPushButton("Find vessel centerline")),
  mRefImageProcessButton(new QPushButton("Find vessel centerline")),
  mRegistrationButton(new QPushButton("I2I registration")),
  mApplyUpdateButton(new QPushButton("Update another image")),
  mMovingImageBox(new QComboBox(this)),
  mRefImageBox(new QComboBox(this)),
  mConnectImageBox(new QComboBox(this)),
  mMovingImageSmoothingCheckBox(new QCheckBox("Smooting", this)),
  mRefImageSmoothingCheckBox(new QCheckBox("Smooting", this)),
  mMovingImageThresholdBox(new QSpinBox(this)),
  mRefImageThresholdBox(new QSpinBox(this)),
  mMovingImageProcess(new QProcess(this)),
  mRefImageProcess(new QProcess(this)),
  mRegistrationProcess(new QProcess(this)),
  mMovingImageDone(false),
  mRefImageDone(false)
{
  
}

ShiftCorrectionWidget::~ShiftCorrectionWidget()
{}

void ShiftCorrectionWidget::init(QString& workingFolder)
{
  mWorkingFolder = workingFolder;
  //QLabel* workingFolderButton = new QPushButton("Set working folder:");
  //QLabel* workingFolderLabel = new QLabel(workingFolder);
  QLineEdit* workingFolderLabel = new QLineEdit(workingFolder);
  connect(workingFolderLabel, SIGNAL(textChanged(const QString&)),
          this, SLOT(changeWorkingFolderSlot(const QString&)));
  QLabel* folderLabel = new QLabel("Folder:");
  
  QLabel* movingImageSmoothingLabel = new QLabel("Moving image");
  //QCheckBox* mMovingImageSmoothingCheckBox = new QCheckBox("Smooting");
  QLabel* movingImageThresholdLabel = new QLabel("Threshold");
  mMovingImageThresholdBox->setMaximum(10000);
  mMovingImageThresholdBox->setValue(100);
  
  QLabel* refImageSmoothingLabel = new QLabel("Reference image");
  //QCheckBox* mRefImageSmoothingCheckBox = new QCheckBox("Smooting");
  QLabel* refImageThresholdLabel = new QLabel("Threshold");
  mRefImageThresholdBox->setMaximum(10000);
  mRefImageThresholdBox->setValue(100);
  
  //First version: Don't disable this button. Will probably do it later
  //mRegistrationButton->setEnabled(false);
  
  mMovingImageProcessButton->setEnabled(false);
  mRefImageProcessButton->setEnabled(false);
  
  QLabel* connectImageLabel = new QLabel("Connect image");
  mApplyUpdateButton->setEnabled(false);
  
  //Connect buttons
  connect(mMovingImageSegmentButton, SIGNAL(clicked()),
          this, SLOT(segmentMovingImageSlot()));
  connect(mRefImageSegmentButton, SIGNAL(clicked()),
          this, SLOT(segmentRefImageSlot()));
  connect(mMovingImageProcessButton, SIGNAL(clicked()),
          this, SLOT(processMovingImageSlot()));
  connect(mRefImageProcessButton, SIGNAL(clicked()),
          this, SLOT(processRefImageSlot()));
  connect(mRegistrationButton, SIGNAL(clicked()),
          this, SLOT(i2iRegistrationSlot()));
  connect(mApplyUpdateButton, SIGNAL(clicked()),
          this, SLOT(applyUpdateSlot()));
  
  //Connect QProcess objects
  connect(mMovingImageProcess, SIGNAL(started()),
          this, SLOT(movingImageRunningSlot()));
  connect(mRefImageProcess, SIGNAL(started()),
          this, SLOT(refImageRunningSlot()));
  connect(mRegistrationProcess, SIGNAL(started()),
          this, SLOT(registrationRunningSlot()));
  connect(mMovingImageProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(movingImageFinishedSlot(int, QProcess::ExitStatus)));
  connect(mRefImageProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(refImageFinishedSlot(int, QProcess::ExitStatus)));
  connect(mRegistrationProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(registrationFinishedSlot(int, QProcess::ExitStatus)));
  
  //Connect QComboBoxes
  connect(mMovingImageBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(movingImageBoxChangedSlot(int)));
  connect(mRefImageBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(refImageBoxChangedSlot(int)));
  
  this->initFileComboBoxes();
  
  //Create layout
  mLayout->setColumnStretch(1, 1);
  
  mLayout->addWidget(folderLabel, 1, 0, Qt::AlignLeft);
  //mLayout->addWidget(workingFolderLabel, 2, 0, 1, 2, Qt::AlignLeft);
  mLayout->addWidget(workingFolderLabel, 1, 1);
  mLayout->setRowStretch(3, 1);
  
  mLayout->addWidget(movingImageSmoothingLabel,     4, 0, Qt::AlignLeft);
  mLayout->addWidget(mMovingImageBox,               4, 1);
  mLayout->addWidget(movingImageThresholdLabel,     5, 0, Qt::AlignLeft);
  mLayout->addWidget(mMovingImageThresholdBox,      5, 1);
  mLayout->addWidget(mMovingImageSmoothingCheckBox, 6, 0, Qt::AlignLeft);
  mLayout->addWidget(mMovingImageSegmentButton,     6, 1);
  mLayout->addWidget(mMovingImageProcessButton,     7, 1);
  
  mLayout->setRowStretch(8, 1);
  
  //mLayout->addSpacing(5);
  mLayout->addWidget(refImageSmoothingLabel,      9,  0 );
  mLayout->addWidget(mRefImageBox,                9,  1);
  mLayout->addWidget(refImageThresholdLabel,     10,  0, Qt::AlignLeft);
  mLayout->addWidget(mRefImageThresholdBox,      10,  1);
  mLayout->addWidget(mRefImageSmoothingCheckBox, 11,  0);
  mLayout->addWidget(mRefImageSegmentButton,     11,  1);
  mLayout->addWidget(mRefImageProcessButton,     12,  1);
  
  mLayout->setRowStretch(13, 1);
  
  //mLayout->addSpacing(5);
  mLayout->addWidget(mRegistrationButton, 14, 1);
  
  mLayout->setRowStretch(15, 1);
  
  //mLayout->addSpacing(5);
  mLayout->addWidget(connectImageLabel,   16, 0);
  mLayout->addWidget(mConnectImageBox,    16, 1);
  mLayout->addWidget(mApplyUpdateButton,  17, 1);
  
  mLayout->setRowStretch(18, 10);
  //mLayout->addStretch(5);
  
  this->setLayout(mLayout);
}
  
void ShiftCorrectionWidget::initFileComboBoxes()
{
	QDir dir(mWorkingFolder);
  dir.setFilter(QDir::Files);
  QStringList nameFilters;
  nameFilters << "*.mha" << "*.mhd";
  dir.setNameFilters(nameFilters);
  QStringList list = dir.entryList();
  mMovingImageBox->clear();
  mMovingImageBox->addItems( list );
  mRefImageBox->clear();
  mRefImageBox->addItems( list );
  mConnectImageBox->clear();
  mConnectImageBox->addItems( list );
}
  
void ShiftCorrectionWidget::segmentImage(QString imageName, 
                                         int thresholdValue,
                                         bool smoothing)
{
  QString filePath = mWorkingFolder+"/"+imageName;
  
  //Read image file
  //ITK
	ImageReaderType::Pointer  reader  = ImageReaderType::New();
	reader->SetFileName(filePath.toLatin1());
	reader->Update();
  
	ImageDirectionType direction = reader->GetOutput()->GetDirection();
	//std::cout << "Matrix = " << std::endl << direction << std::endl;
	int i,j;
	vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
	for (i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			matrix->SetElement(i, j, direction.GetVnlMatrix()[i][j]);
			//std::cout << "Matrix element = " << matrix->GetElement(i,j) << std::endl;
		}
	}
  
  ImageType::Pointer data = reader->GetOutput();
  
  //Smooting
  if(smoothing)
  {
    typedef itk::SmoothingRecursiveGaussianImageFilter<ImageType, ImageType> smoothingFilterType;
    smoothingFilterType::Pointer smoohingFilter = smoothingFilterType::New();
    smoohingFilter->SetSigma(0.5);
    smoohingFilter->SetInput(data);
    smoohingFilter->Update();
    data = smoohingFilter->GetOutput();
  }
  
  //Thresholding
  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> thresholdFilterType;
  thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
  thresholdFilter->SetInput(data);
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->SetInsideValue(1);
  thresholdFilter->SetLowerThreshold(thresholdValue);
  thresholdFilter->Update();
  data = thresholdFilter->GetOutput();
  
  //Test writer
  QString outName = filePath;
  if(outName.endsWith(".mhd"))
    outName.replace(QString(".mhd"), QString(""));
  if(outName.endsWith(".mha"))
    outName.replace(QString(".mha"), QString(""));
  
  QString outFileName = outName+"_segm_converted.mhd";
  QString rawFileName = outName+"_segm_converted.raw";
  
  
  //Convert ITK to VTK
  itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
  //itkToVtkFilter->SetInput(data);
  itkToVtkFilter->SetInput(thresholdFilter->GetOutput());
  itkToVtkFilter->Update();
  
  //Get 3D view
  View3D* view = ViewManager::getInstance()->get3DView("View3D_1");
  
  //Show converted volume = empty?
  /*ssc::ImagePtr image = ssc::ImagePtr(new ssc::Image(outName.toStdString()+"_segm_volume", 
                                                     itkToVtkFilter->GetOutput()));
  ssc::VolumetricRepPtr volumetricRep(ssc::VolumetricRep::New(outName.toStdString()+"_segm_volume"));
  volumetricRep->setImage(image);
  
  //Crash???
  view->setRep(volumetricRep);*/
  
  
  //test Save vtk object
  /*typedef vtkSmartPointer<vtkMetaImageWriter> vtkMetaImageWriterPtr;
  vtkMetaImageWriterPtr vtkWriter = vtkMetaImageWriterPtr::New();
	vtkWriter->SetInput(itkToVtkFilter->GetOutput());
	vtkWriter->SetFileName( outFileName.toLatin1() );
	vtkWriter->SetRAWFileName( rawFileName.toLatin1() );
  vtkWriter->SetCompression(false);
	vtkWriter->Update();
  vtkWriter->Write();*/
  
  
  outFileName = outName+"_segm.mhd";
  
  //Create vtkPolyData
  /*vtkImageToPolyDataFilter* convert = vtkImageToPolyDataFilter::New();
  convert->SetInput(itkToVtkFilter->GetOutput());
  convert->SetColorModeToLinear256();
  convert->Update();*/
  
  vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
  convert->SetInput(itkToVtkFilter->GetOutput());
  //convert->SetValue(0, 150);
  convert->Update();
  //messageManager()->sendInfo("Number of contours: "+QString::number(convert->GetNumberOfContours()).toStdString());

  vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
  cubesPolyData = convert->GetOutput();
  
  //print
  //itkToVtkFilter->GetOutput()->Print(std::cout);
  cubesPolyData->Print(std::cout);
  //vtkPolyData* cubesPolyData = convert->GetOutput();
  
  ssc::MeshPtr surface = ssc::MeshPtr(new ssc::Mesh(outName.toStdString()+"_segm"));
  surface->setVtkPolyData(cubesPolyData);
  ssc::GeometricRepPtr surfaceRep(ssc::GeometricRep::New(outName.toStdString()+"_segm"));
  surfaceRep->setMesh(surface);
    
  view->addRep(surfaceRep);
  
  //Cone test
  typedef vtkSmartPointer<vtkConeSource> vtkConeSourcePtr;
  vtkConeSourcePtr coneSource = vtkConeSource::New();
  coneSource->SetResolution(25);
  coneSource->SetRadius(10);
  coneSource->SetHeight(100);
  
  coneSource->SetDirection(0,0,1);
  double newCenter[3];
  coneSource->GetCenter(newCenter);
  newCenter[2] = newCenter[2] - coneSource->GetHeight()/2;
  coneSource->SetCenter(newCenter);
  
  //Cone rep visialization
  ssc::MeshPtr coneSurface = ssc::MeshPtr(new ssc::Mesh("cone"));
  coneSurface->setVtkPolyData(coneSource->GetOutput());
  ssc::GeometricRepPtr coneSurfaceRep(ssc::GeometricRep::New("cone"));
  coneSurfaceRep->setMesh(coneSurface);
  view->addRep(coneSurfaceRep);
  
  //print
  coneSource->Update();
  coneSource->GetOutput()->Print(std::cout);
  //vtkPolyData* conePolyData = coneSource->GetOutput();
  
  //vtkPolyData* surface = convert->GetOutput();
  
  //test: Show surface
  /*vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInput(surface);
  mapper->Update();
  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  actor->SetVisibility(true);
  actor->SetUserMatrix(matrix);
  
  ViewManager::getInstance()->get3DView("View3D_1")->getRenderer()->AddActor(actor);
  */
  
  //Test save ITK object
	ImageWriterType::Pointer writer = ImageWriterType::New();
	writer->SetInput(data);
	writer->SetFileName( outFileName.toLatin1() );
	writer->Update();
}
  
void ShiftCorrectionWidget::processImage(QString imageName)
{
  QString filePath = mWorkingFolder+"/"+imageName;
  QString outName = filePath;
  if(outName.endsWith(".mhd"))
    outName.replace(QString(".mhd"), QString(""));
  if(outName.endsWith(".mha"))
    outName.replace(QString(".mha"), QString(""));
  QString fileName = outName+"_segm.mhd";
  
  //Read image file
  //ITK
	ImageReaderType::Pointer  reader  = ImageReaderType::New();
	reader->SetFileName(fileName.toLatin1());
	reader->Update();
  
	ImageDirectionType direction = reader->GetOutput()->GetDirection();
	//std::cout << "Matrix = " << std::endl << direction << std::endl;
	int i,j;
	vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
	for (i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			matrix->SetElement(i, j, direction.GetVnlMatrix()[i][j]);
			//std::cout << "Matrix element = " << matrix->GetElement(i,j) << std::endl;
		}
	}
  
  ImageType::Pointer data = reader->GetOutput();
  
   //Centerline extraction
   typedef itk::BinaryThinningImageFilter<ImageType, ImageType> centerlineFilterType;
   centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
   centerlineFilter->SetInput(data);
   centerlineFilter->Update();
   data = centerlineFilter->GetOutput();
   
   //Test writer
   fileName = outName+"_center.mhd";
  
	ImageWriterType::Pointer writer = ImageWriterType::New();
   writer->SetInput(data);
   writer->SetFileName( fileName.toLatin1() );
   writer->Update();
  
    //Convert ITK to VTK
   itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
   itkToVtkFilter->SetInput(data);
   
  //Create vtkPolyData
  vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
  convert->SetInput(itkToVtkFilter->GetOutput());
  convert->Update();
  vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
  cubesPolyData = convert->GetOutput();
  
   /*vtkImageToPolyDataFilter* convert = vtkImageToPolyDataFilter::New();
   convert->SetInput(itkToVtkFilter->GetOutput());
   convert->Update();
   vtkPolyData* surface = convert->GetOutput();*/
   
   //test: Show surface
   /*vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
   mapper->SetInput(surface);
   vtkActor* actor = vtkActor::New();
   actor->SetMapper(mapper);*/
     
  //Show surface
  ssc::MeshPtr surface = ssc::MeshPtr(new ssc::Mesh(outName.toStdString()+"_segm"));
  surface->setVtkPolyData(cubesPolyData);
  ssc::GeometricRepPtr surfaceRep(ssc::GeometricRep::New(outName.toStdString()+"_segm"));
  surfaceRep->setMesh(surface);
  
  //Get 3D view
  View3D* view = ViewManager::getInstance()->get3DView("View3D_1");
  view->addRep(surfaceRep);
  
  //VTK
  /*vtkMetaImageReader* reader = vtkMetaImageReader::New();
   //reader->SetFileName(imageName.toStdString().c_str());
   reader->SetFileName(filePath.toLatin1());
   reader->Update();
   
   vtkImageData* data = reader->GetOutput();
   
   if (smoothing)
   {
   vtkImageGaussianSmooth* gaussian = vtkImageGaussianSmooth::New();
   gaussian->SetDimensionality(3);
   gaussian->SetInput(data);
   data = gaussian->GetOutput();
   }
   
   vtkImageThreshold* threshold = vtkImageThreshold::New();
   threshold->ThresholdByUpper(thresholdValue);
   threshold->SetInValue(255);
   threshold->SetOutValue(0);
   threshold->ReplaceOutOn();
   threshold->ReplaceInOn();
   threshold->SetInput(data);
   
   //Test writer
   vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
   writer->SetInput(data);
   //QDir dir(filePath);
   QString outFileName = filePath;
   if(outFileName.endsWith(".mhd"))
   outFileName.replace(QString(".mhd"), QString("_segm"));
   if(outFileName.endsWith(".mha"))
   outFileName.replace(QString(".mha"), QString("_segm"));
   
   writer->SetFileName(outFileName.toStdString().c_str());
   writer->SetFileDimensionality(3);
   writer->SetFileName(std::string(outFileName.toStdString()+".mhd").c_str());
   writer->SetRAWFileName(std::string(outFileName.toStdString()+".raw").c_str());
   writer->SetCompression(false);
   writer->Update();
   writer->Write();*/
  
  //Get MINC file name
  //QString rawFilepath = QString::fromStdString(mFilePath);
  /*  QString mincFilePath = filePath;
   if(mincFilePath.endsWith(".mhd"))
   mincFilePath.replace(QString(".mhd"), QString(".mnc"));
   if(mincFilePath.endsWith(".mha"))
   mincFilePath.replace(QString(".mha"), QString(".mnc"));*/
  
  //Save data in MINC format
  /*	const    unsigned int    Dimension = 3;
   typedef  float  PixelType;
   typedef itk::Image< PixelType, Dimension >  ImageType;
   typedef ImageType::DirectionType ImageDirectionType;
   ImageDirectionType direction = reader->GetOutput()->GetDirection();
   //std::cout << "Matrix = " << std::endl << direction << std::endl;
   int i,j;
   vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
   for (i=0;i<3;i++)
   {
   for(j=0;j<3;j++)
   {
   matrix->SetElement(i, j, direction.GetVnlMatrix()[i][j]);
   //std::cout << "Matrix element = " << matrix->GetElement(i,j) << std::endl;
   }
   }
   vtkMINCImageWriter* writer = vtkMINCImageWriter::New();
   writer->SetInput(data);
   writer->SetFileName(filePath);
   writer->SetDirectionCosines(matrix);
   writer->Write();*/
  
  /*QString program = "sleep"; //test
   QStringList arguments;
   arguments << "3";
   process->start(program, arguments);*/ 
  
}


void ShiftCorrectionWidget::segmentMovingImageSlot()
{
  //TODO: Run in separate thread
  mMovingImageSegmentButton->setText("Running...");
  mMovingImageSegmentButton->setEnabled(false);
  mMovingImageSegmentButton->update();
  this->segmentImage(mMovingImageBox->currentText(),
                     mMovingImageThresholdBox->value(),
                     mMovingImageSmoothingCheckBox->isChecked());
  mMovingImageSegmentButton->setText("Segmentation done");
  mMovingImageSegmentButton->setEnabled(true);
  mMovingImageProcessButton->setEnabled(true);
}
  
void ShiftCorrectionWidget::segmentRefImageSlot()
{
  //TODO: Run in separate thread
  mRefImageSegmentButton->setText("Running...");
  mRefImageSegmentButton->setEnabled(false);
  mRefImageSegmentButton->update();
  this->segmentImage(mRefImageBox->currentText(),
                     mRefImageThresholdBox->value(),
                     mRefImageSmoothingCheckBox->isChecked());
  mRefImageSegmentButton->setText("Segmentation done");
  mRefImageSegmentButton->setEnabled(true);
  mRefImageProcessButton->setEnabled(true);
}
  
void ShiftCorrectionWidget::processMovingImageSlot()
{
  //TODO: Run in separate thread
  
  mMovingImageProcessButton->setText("Running...");
  mMovingImageProcessButton->setEnabled(false);
  mMovingImageProcessButton->update();
  processImage(mMovingImageBox->currentText());
  mMovingImageProcessButton->setText("Centerline done");
  mMovingImageProcessButton->setEnabled(true);
}

void ShiftCorrectionWidget::processRefImageSlot()
{
  //TODO: Run in separate thread
  mRefImageProcessButton->setText("Running...");
  mRefImageProcessButton->setEnabled(false);
  mRefImageProcessButton->update();
  processImage(mRefImageBox->currentText());
  mRefImageProcessButton->setText("Centerline done");
  mRefImageProcessButton->setEnabled(true);
}
void ShiftCorrectionWidget::i2iRegistrationSlot()
{
  QString program = "sleep"; //test
  QStringList arguments;
  arguments << "2";
  mRegistrationProcess->start(program, arguments);
}
void ShiftCorrectionWidget::applyUpdateSlot()
{
  
}
void ShiftCorrectionWidget::movingImageRunningSlot()
{
  mMovingImageProcessButton->setText("Running...");
  mMovingImageProcessButton->setEnabled(false);
}
void ShiftCorrectionWidget::refImageRunningSlot()
{
  mRefImageProcessButton->setText("Running...");
  mRefImageProcessButton->setEnabled(false);
}
void ShiftCorrectionWidget::registrationRunningSlot()
{
  mRegistrationButton->setText("Running...");
  mRegistrationButton->setEnabled(false);
}
void ShiftCorrectionWidget::movingImageFinishedSlot(int, QProcess::ExitStatus)
{
  mMovingImageProcessButton->setText("Centerline Done");
  mMovingImageProcessButton->setEnabled(true);
  mMovingImageDone = true;
  if(mRefImageDone)
    mRegistrationButton->setEnabled(true);
}
void ShiftCorrectionWidget::refImageFinishedSlot(int, QProcess::ExitStatus)
{
  mRefImageProcessButton->setText("Centerline Done");
  mRefImageProcessButton->setEnabled(true);
  mRefImageDone = true;
  if(mMovingImageDone)
    mRegistrationButton->setEnabled(true);
}
void ShiftCorrectionWidget::registrationFinishedSlot(int, QProcess::ExitStatus)
{
  mRegistrationButton->setText("I2I reg Done");
  mRegistrationButton->setEnabled(true);
  
  mApplyUpdateButton->setEnabled(true);
}
  
void ShiftCorrectionWidget::changeWorkingFolderSlot(const QString& folder)
{
  mWorkingFolder = folder;
  initFileComboBoxes();
}
  
void ShiftCorrectionWidget::movingImageBoxChangedSlot(int index)
{
  this->updateProcessButton(mMovingImageBox, mMovingImageProcessButton);
}
  
void ShiftCorrectionWidget::refImageBoxChangedSlot(int index)
{
  this->updateProcessButton(mRefImageBox, mRefImageProcessButton);
}
  
void ShiftCorrectionWidget::updateProcessButton(QComboBox* imageBox,
                                                QPushButton* button)
{
  QString filePath = mWorkingFolder+"/"+imageBox->currentText();
  
  if(filePath.endsWith(".mhd"))
    filePath.replace(QString(".mhd"), QString(""));
    if(filePath.endsWith(".mha"))
      filePath.replace(QString(".mha"), QString(""));
  QString fileName = filePath+"_segm.mhd";
  
  QFile file(fileName);
  if(file.exists())
    button->setEnabled(true);
  else
    button->setEnabled(false);
}
      
}//namespace
