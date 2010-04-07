//===========================================================================
//
// File: usreconstruction.cpp
//
// Created: 12.0.2007
//
// Revision: $Id:$
//
/// \file
///       Ultrasound reconstruction algorithms
/// \author Ole Vegard Solberg
//===========================================================================

#include "cxUsReconstruction.h"

#include <time.h>
#include <vtkImageShiftScale.h>
#include <vtkMetaImageWriter.h>
#include <iostream>

UsReconstruction::UsReconstruction()
{
  m_calibration = TransformType::New();
  m_calibration->SetIdentity();
}

UsReconstruction::~UsReconstruction()
{
}

/*void UsReconstruction::importPositions(
                                Input2DUSTemplate<char>* et, 
                                std::vector<std::vector<float> > *positions)
{
  inputPositions.resize(et->numberOfRecords);
  unsigned long record = 0;
  while(record < et->numberOfRecords)
  {
    inputPositions.at(record)[0] = positions->at(record)[0];
    inputPositions.at(record)[1] = positions->at(record)[1];
    inputPositions.at(record)[2] = positions->at(record)[2];
    inputPositions.at(record)[3] = positions->at(record)[3];
    inputPositions.at(record)[4] = positions->at(record)[4];
    inputPositions.at(record)[5] = positions->at(record)[5];
    inputPositions.at(record)[6] = positions->at(record)[6];
    inputPositions.at(record)[7] = positions->at(record)[7];
    inputPositions.at(record)[8] = positions->at(record)[8];
    inputPositions.at(record)[9] = positions->at(record)[9];
    inputPositions.at(record)[10] = positions->at(record)[10];
    inputPositions.at(record)[11] = positions->at(record)[11];
    record++;
  }//while
}*/

//void UsReconstruction::findVolumeOrientationAndRotation(Input2DUSTemplate<char>* et)
void UsReconstruction::findVolumeOrientationAndRotation(Input2DUS* usData,
                                                        bool alignMiddle,
                                                        std::string 
                                                        mTargetVolumeTransformFile)
{
  //std::cout << "UsReconstruction::findVolumeOrientationAndRotation() called" << 
  //std::endl;
  
  // Align target volume with middle input slice -->
  // et->finalPositions->at(et->numberOfDataRecords/2)
  
  // Transform from input image positions to output volume position
  m_targetVolumeTransform = TransformType::New();
  
  
  if (mTargetVolumeTransformFile != "")
  {
    m_targetVolumeTransform = AffineTransformType::New();
    readTransform(m_targetVolumeTransform, 
                  //m_volumeSize, 
                  mTargetVolumeTransformFile);
    return;
  }
  //TransformType::Pointer m_testTransform = TransformType::New();
  
  // Temporary variables
  // Record number for middle input slice
  //unsigned int r = usData->numberOfDataRecords/2; //et
  unsigned int r = usData->numberOfRecords/2;
  //std::cout << "middle: " << r << std::endl;
  // Parameters for target volume 
  // Must be initialized before use - use GetParameters from TransformType
  TransformType::ParametersType params = 
    m_targetVolumeTransform->GetParameters();
  
  //readParams(params, usData->finalPositions->at(r)); //et
  readParams(params, usData->positionVector.at(r));
  
  m_targetVolumeTransform->SetParameters(params);
  
  m_targetVolumeTransform->Compose(m_calibration, 1);
  
  //m_targetVolumeTransform->Compose(m_moveOrigo, 0);//Test
  
  TransformType::InputPointType newRotatioCenter;
  TransformType::OutputVectorType offset;
  //newRotatioCenter = m_targetVolumeTransform->GetCenter();
  offset = m_targetVolumeTransform->GetOffset();
  newRotatioCenter[0] = offset[0];
  newRotatioCenter[1] = offset[1];
  newRotatioCenter[2] = offset[2];
  //offset[0] = -offset[0];
  //offset[1] = -offset[1];
  //offset[2] = -offset[2];
  offset[0] = 0;
  offset[1] = 0;
  offset[2] = 0;
  //std::cout << "frame: " << r << " usData->numberOfRecords: ";
  //std::cout << usData->numberOfRecords << std::endl;
  //std::cout << "params: " << params << std::endl;
  //std::cout << "m_targetVolumeTransform: " << m_targetVolumeTransform << std::endl;
  //std::cout << "newRotatioCenter: " << newRotatioCenter << std::endl;
  
  // Move origo (Center of rotation)
  //m_targetVolumeTransform->SetCenter(newRotatioCenter);
  // Invert
  m_targetVolumeTransform->GetInverse(m_targetVolumeTransform);
  //m_targetVolumeTransform->SetOffset(offset);
  
  //test
  //m_testTransform->SetParameters(params);
  //m_testTransform->Compose(m_targetVolumeTransform,1);
  //std::cout << "m_testTransform: " << m_testTransform << std::endl;
  
  // use only identity if we are not to align target volume with middle slice
  if (!alignMiddle)
    m_targetVolumeTransform->SetIdentity();
    //m_targetVolumeTransform = m_calibration;
}

void UsReconstruction::setCalibration(TransformType::Pointer cal)
{
  m_calibration->SetParameters(cal->GetParameters());
}

//void UsReconstruction::findVolumeSize(Input2DUSTemplate<char>* et
void UsReconstruction::findVolumeSize(Input2DUS* usData, 
                                      std::string outputFileName)
{  
//{
  //std::cout << "UsReconstruction::findVolumeSize() called" << std::endl;
  TransformType::Pointer transform = TransformType::New();

  TransformType::InputPointType p1, p2, p3, p4;
  // p1-----p2  z    y-->   | -  0   + |
  // |      |   |           |          |
  // p3-----p4  V           |    +     |
  //p1[0] = 0; p1[1] = 0; p1[2] = 0; 
  
  //     p1     //
  //   /    \   //
  // p2-_p3_-p4 //
  
  // polar to cartesian translation
  //  x = depth * cos(angle);
  //  y = depth * sin(angle);
  
  //test
  //usData->polar = false;
  
  // Polar coordinates
  if (usData->polar)
  {
    std::cout << "Using polar coordinates " << std::endl;
    double depth = usData->startDepth + usData->samples * usData->depthIncrement;
    double stopAngle = usData->startAngle + usData->angleIncreament * usData->beams;
    double middleAngle = usData->startAngle + usData->angleIncreament * usData->beams / 2;
    p1[0] = 0; p1[1] = 0; p1[2] = 0; 
    p2[0] = 0; 
    p2[1] = depth * sin(usData->startAngle); 
    p2[2] = depth * cos(usData->startAngle); 
    p3[0] = 0; 
    p3[1] = depth * sin(middleAngle); 
    p3[2] = depth * cos(middleAngle); 
    p4[0] = 0; 
    p4[1] = depth * sin(stopAngle); 
    p4[2] = depth * cos(stopAngle); 
  } else {
    //cartesian
    //std::cout << "Using cartesian coordinates " << std::endl;
    
    //Multiply with spacing
    //double beamWith =    usData->vtkData->GetSpacing()[0];
    //double sampleDepth = usData->vtkData->GetSpacing()[1];
    std::cout << "with: " << usData->beamWith << " depth: " 
      << usData->sampleDepth << std::endl;
    double yMinMax = usData->beams/2 * usData->beamWith;
    double zMax = usData->samples * usData->sampleDepth;
    p1[0] = 0; p1[1] = -yMinMax; p1[2] = 0; 
    p2[0] = 0; p2[1] =  yMinMax; p2[2] = 0; 
    p3[0] = 0; p3[1] = -yMinMax; p3[2] = zMax; 
    p4[0] = 0; p4[1] =  yMinMax; p4[2] = zMax; 
  }
  
  TransformType::InputPointType p1out, p2out, p3out, p4out;
  
  // Must be initialized
  TransformType::ParametersType params = transform->GetParameters();
  
  // Volume extremal values
  double rcMaxX = -100000;
  double rcMaxY = -100000;
  double rcMaxZ = -100000;
  double rcMinX = 100000;
  double rcMinY = 100000;
  double rcMinZ = 100000;
  
  //std::cout << "*** calibration *** " << m_calibration << std::endl;
  //std::cout << "*** transform *** " << m_targetVolumeTransform << std::endl;
  
  //for(unsigned int r = 0; r < usData->numberOfDataRecords; r++) //et
  for(unsigned int r = 0; r < usData->numberOfRecords; r++)
  {
    //readParams(params, usData->finalPositions->at(r)); //et
    readParams(params, usData->positionVector.at(r));
    transform->SetParameters(params);
    
    //!!-ovs: Try without the result from findVolumeOrientationAndRotation()
    // Volume is larger, when it should be smaller ???
    // The axes are probably wrong
    //transform->Compose(m_targetVolumeTransform, 1);
    //transform->Compose(m_targetVolumeTransform, 0); // old reconstruction
    transform->Compose(m_calibration, 1);
    transform->Compose(m_targetVolumeTransform, 0);
        
    p1out = transform->TransformPoint(p1);
    p2out = transform->TransformPoint(p2);
    p3out = transform->TransformPoint(p3);
    p4out = transform->TransformPoint(p4);
    
    // testcode
    //-----------------
    //if ((r >= usData->numberOfDataRecords / 2 - 1) && 
    //    (r <= usData->numberOfDataRecords / 2 + 1))
    //if (r == 0)
    /*if (r == (usData->numberOfRecords / 2) )
    {
      TransformType::Pointer testTransform = TransformType::New();
      TransformType::InputPointType pt1, pt2, pt3, pt4;
      testTransform->SetParameters(params);
      
      pt1[0] = 0; pt1[1] = 0; pt1[2] = 0; 
      pt2 = testTransform->TransformPoint(pt1);
      //pt2 = testTransform->TransformPoint(p2);
      pt3 = testTransform->TransformPoint(p3);
      //pt4 = testTransform->TransformPoint(p4);
      
      std::cout << "*** Input position *** " << usData->positionVector.at(r) 
      << std::endl;
      std::cout << "frame: " << r << " usData->numberOfRecords: ";
      std::cout << usData->numberOfRecords << std::endl;
      std::cout << "params: " << params << std::endl;
      std::cout << "transform: " << transform << std::endl;
      
      std::cout << "p1 in:  " << pt1;
      std::cout << "  p1out: " << p1out << std::endl;
      //std::cout << "2 in:  " << p2;
      //std::cout << "  p2out: " << p2out << std::endl;
    //  std::cout << "3 in:  " << p3;
    //  std::cout << "  p3out: " << pt3 << std::endl;
    //  std::cout << "4 in:  " << p4;
    //  std::cout << "  p4out: " << p4out << std::endl << std::endl;
    }*/
    //-----------------
        
    // Find extreme values
    rcMinX = min(rcMinX, p1out[0], p2out[0], p3out[0], p4out[0]);
    rcMaxX = max(rcMaxX, p1out[0], p2out[0], p3out[0], p4out[0]);
    
    rcMinY = min(rcMinY, p1out[1], p2out[1], p3out[1], p4out[1]);
    rcMaxY = max(rcMaxY, p1out[1], p2out[1], p3out[1], p4out[1]);
    
    rcMinZ = min(rcMinZ, p1out[2], p2out[2], p3out[2], p4out[2]);
    rcMaxZ = max(rcMaxZ, p1out[2], p2out[2], p3out[2], p4out[2]);
    
  }
  double rcLengthX, rcLengthY, rcLengthZ;
  rcLengthX = rcMaxX - rcMinX;// +1;
  rcLengthY = rcMaxY - rcMinY;// + 1;
  rcLengthZ = rcMaxZ - rcMinZ;// + 1;
  
  m_volumeSize[0] = (unsigned int)(ceil(rcLengthX / m_volumeSpacing[0]));
  m_volumeSize[1] = (unsigned int)(ceil(rcLengthY / m_volumeSpacing[1]));
  m_volumeSize[2] = (unsigned int)(ceil(rcLengthZ / m_volumeSpacing[2]));
  
//  std::cout << "rcMinX / rcMaxX / rcLengthX : "<< rcMinX << " / " << 
//    rcMaxX  << " / " << rcLengthX << std::endl;
//  std::cout << "rcMinY / rcMaxY / rcLengthY : "<< rcMinY << " / " << 
//    rcMaxY  << " / " << rcLengthY << std::endl;
//  std::cout << "rcMinZ / rcMaxZ / rcLengthZ : "<< rcMinZ << " / " << 
//    rcMaxZ  << " / " << rcLengthZ << std::endl;
    
  //std::cout << "volume spacing: "<< m_volumeSpacing[0] << " " << 
  //  m_volumeSpacing[1]  << " " << m_volumeSpacing[2] << std::endl;
  
  
  //if (!transformImported)
  //{
    /*TransformType::Pointer*/
    m_moveOrigo = TransformType::New();
    m_moveOrigo->SetIdentity();
    //init variable
    TransformType::ParametersType origoParams = m_moveOrigo->GetParameters();
    origoParams[9] = rcMinX;
    origoParams[10] = rcMinY;
    origoParams[11] = rcMinZ;
    m_moveOrigo->SetParameters(origoParams);
    m_moveOrigo->GetInverse(m_moveOrigo);
    std::cout << "m_moveOrigo: " << m_moveOrigo << std::endl;
    
    //Update transform. May want to store these into different variables
  //  std::cout << "m_targetVolumeTransform: " << m_targetVolumeTransform << std::endl;
    
    // Add movement of origo to m_targetVolumeTransform
    //m_targetVolumeTransform->GetInverse(m_targetVolumeTransform);
    m_targetVolumeTransform->Compose(m_moveOrigo, 0);
    //m_targetVolumeTransform->GetInverse(m_targetVolumeTransform);
  //  std::cout << "m_targetVolumeTransform after move: " << m_targetVolumeTransform << std::endl;
    
    // Write m_targetVolumeTransform to file
    std::stringstream trfmFileName;
    trfmFileName << outputFileName << ".trf";
    writeTransform(m_targetVolumeTransform, m_volumeSize, trfmFileName.str().c_str());
    
    //std::stringstream randomFileName;
    //randomFileName << outputFileName << "_p" << usData->randomNumbersPercentage << ".rnd";
    //findRandomNumbers(usData, randomFileName.str().c_str());
  
  //}
//  std::cout << "volume size: "<< m_volumeSize[0] << " " << 
//  m_volumeSize[1]  << " " << m_volumeSize[2] << std::endl;
  
  //std::cout << "*** transform *** " << m_targetVolumeTransform << std::endl;
  
/*	//std::ofstream mhaFile(mhaFilename.c_str(), std::ios::out);
	FILE* mhaFile = fopen(mhaFilename.c_str(), "w");
	fprintf(mhaFile, "NDims = 3\n"
          "DimSize = %i %i %i\n"
          "ElementType = %s\n"
          "ElementSpacing = %f %f %f\n"
          "ElementByteOrderMSB = %s\n"
          "ElementDataFile = %s\n",
          m_volumeSize[0], m_volumeSize[1], m_volumeSize[2],
          //"MET_USHORT",//ITK
          "MET_UCHAR",//VTK
          m_volumeSpacing[0], m_volumeSpacing[1], m_volumeSpacing[2],
          //bigEndianFile_ ? "True" : "False",
          "False",
          rawFilename.c_str());
  //"test.raw");
	fclose(mhaFile);
	
	mhaFilename.clear();
  
  // Write .raw file
  vtkImageWriter* imgw = vtkImageWriter::New();
  //imgw->SetFileName(dirname_ + "/" + sfilename);
  //imgw->SetFileName("test.raw");
  imgw->SetFileName(rawFilename.c_str());
  imgw->SetInput(volume);
  imgw->SetFileDimensionality(3);
  imgw->Write();*/
  
}

/*
double UsReconstruction::min(double x, double y)
{
  if (x < y) return x;
  else return y;
}
double UsReconstruction::max(double x, double y)
{
  if (x > y) return x;
  else return y;
}
double UsReconstruction::min(double a, double b, double c, double d, double e)
{
  return min(min(min(a,b),min(c,d)),e);
}
double UsReconstruction::max(double a, double b, double c, double d, double e)
{
  return max(max(max(a,b),max(c,d)),e);
}*/

void UsReconstruction::allocateVolume(TempVolumeType::Pointer& volume)
{
  TempVolumeType::RegionType region;
  TempVolumeType::IndexType index;
  index[0] = 0; 
  index[1] = 0; 
  index[2] = 0;
  region.SetIndex(index);
  region.SetSize(m_volumeSize);
  
  
  TempVolumeType::InternalPixelType origin[TempVolumeType::ImageDimension];
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  
  volume->SetRegions(region);
  volume->SetSpacing(m_volumeSpacing);
  volume->SetOrigin(origin);
  volume->Allocate();
  volume->FillBuffer(0);
}


/*void UsReconstruction::findInputPoint(Input2DUSTemplate<char>* et, 
                                      TransformType::InputPointType& point, 
                                      bool polar)
{
  // May possibly speed up this by incremental additions
  if (polar)
  {
    double depth = et->startDepth + et->depthIncrement * sample;
    double angle = et->startAngle + et->angleIncreament * bean;
    point[0] = 0;
    point[1] = depth * sin(angle);
    point[2] = depth * cos(angle);
  } else {  // Cartesian
    // See also old code to do this correctly
    point[0] = 0;
    point[1] = (beam - et->beams/2.0)  * m_volumeSpacing[1];
    point[2] = sample * m_volumeSpacing[2];
  }
}*/

//vtkImageData* UsReconstruction::createPNNVolume(Input2DUSTemplate<char>* et, 
//                                                double spacing)
vtkImageData* UsReconstruction::reconstructVolume(Input2DUS* usData, 
                                                  std::string outputFileName,
                                                  ReconstructionTypeEnum type,
                                                  double spacing,
                                                  int interpolationSteps,
                                                  bool alignMiddle,
                                                  std::string 
                                                  mTargetVolumeTransformFile,
                                                  //std::string settingsFile,
                                                  bool maxRange,
                                                  unsigned int removeFromRecord)
{  
  std::cout << "UsReconstruction::reconstructVolume called" << std::endl;
  std::cout << "removeFromRecord: " << removeFromRecord << std::endl;
  unsigned int record = 0;
  signed short beam = 0;
  unsigned int sample = 0;
  
  int inputMin = UCHAR_MAX;
  int inputMax = 0;
  
  //readSettings(usData, settingsFile);
  
  // Must be initialized based on input data
  m_volumeSpacing[0] = spacing;
  m_volumeSpacing[1] = spacing;
  m_volumeSpacing[2] = spacing;
  
  m_moveOrigo = TransformType::New();
  m_moveOrigo->SetIdentity();
  TransformType::ParametersType origoParams = m_moveOrigo->GetParameters();
  // Changing coordinate systems
  origoParams[9] = 0;
  origoParams[10] =  - (usData->mOffset[0] - usData->vtkData->GetDimensions()[0] 
                        / 2) * usData->vtkData->GetSpacing()[0];
  origoParams[11] =  - usData->mOffset[1] * usData->vtkData->GetSpacing()[1];
  m_moveOrigo->SetParameters(origoParams);
  
  //m_moveOrigo->GetInverse(m_moveOrigo);
  //std::cout << "Offset: " << usData->mOffset[0] << " " << usData->mOffset[1] << " " << usData->mOffset[2] << std::endl;
  //std::cout << "origoParams: " << origoParams[9] << " " << origoParams[10] << " " << origoParams[11] << std::endl;
  //std::cout << "Spacing: " << usData->vtkData->GetSpacing()[0] << " " << usData->vtkData->GetSpacing()[1] << " " << usData->vtkData->GetSpacing()[2] << std::endl;
  //std::cout << "GetDimensions: " << usData->vtkData->GetDimensions()[0] << " " << usData->vtkData->GetDimensions()[1] << " " << usData->vtkData->GetDimensions()[2] << std::endl;
  //std::cout << "m_moveOrigo: " << m_moveOrigo;
  //std::cout << "calibration before: " << usData->calibration;
  
  usData->calibration->Compose(m_moveOrigo,1);
  //std::cout << "calibration after: " << usData->calibration;
  
  //Read calibration from usData
  setCalibration(usData->calibration);
  
  // 
  clock_t t1, t2;
  
  
  //Initialize m_targetVolumeTransform
  //findVolumeOrientationAndRotation(et);
  findVolumeOrientationAndRotation(usData, 
                                   alignMiddle, 
                                   mTargetVolumeTransformFile);
  
  t1 = clock();
  
  bool transformImported = false;
  if (mTargetVolumeTransformFile != "")
    transformImported = true;    
  
  //findVolumeSize(et);
  
  
  usData->polar = false;
  if (!transformImported)
    findVolumeSize(usData, outputFileName);

  // Create random numbers if they are not imported
  if(!usData->hasRandomNumbers)
  {
    std::stringstream randomFileName;
    randomFileName << outputFileName << ".rnd";
    findRandomNumbers(usData, randomFileName.str().c_str());
  }
  
  std::cout << "volume size: "<< m_volumeSize[0] << " " << 
  m_volumeSize[1]  << " " << m_volumeSize[2] << std::endl;
    
  // Create the temporary target volume
  
  //test: removed ITK components
  /*TempVolumeType::Pointer targetVolume = TempVolumeType::New();
  allocateVolume(targetVolume);

  TempVolumeType::IndexType indexStart;
  indexStart[0] = 0;
  indexStart[1] = 0;
  indexStart[2] = 0;
  double *targetVolumePtr = &targetVolume->GetPixel(indexStart);
  */
  // VTK
  //Init vtkImageData
  vtkImageData* targetVolume = vtkImageData::New();    
  //targetVolume->SetScalarTypeToUnsignedShort(); 
  
  //targetVolume->Initialize();
  if (type == PNN)
    targetVolume->SetScalarTypeToUnsignedChar();
  else //kernel
    targetVolume->SetScalarTypeToDouble();
  
  //Test SetSpacing
  targetVolume->SetSpacing(spacing, spacing, spacing);
  targetVolume->SetNumberOfScalarComponents(1);
  targetVolume->SetExtent(0, m_volumeSize[0]-1, 
                          0, m_volumeSize[1]-1, 
                          0, m_volumeSize[2]-1);	
  targetVolume->AllocateScalars();
  double *targetVolumeDoublePtr = 
            static_cast<double*>(targetVolume->GetScalarPointer());
  unsigned char *targetVolumeCharPtr = 
    static_cast<unsigned char*>(targetVolume->GetScalarPointer());
  
  //-----test--------
  //t2 = clock();
  //double dif = (t2 - t1);
  //dif = dif / CLOCKS_PER_SEC;
  //std::cout << "Time: " << dif << " sec" << std::endl;
  //-----------------
    
  
  // Init transform variables
  TransformType::Pointer sliceTransform = TransformType::New();
  //Init
  TransformType::ParametersType sliceParams = sliceTransform->GetParameters();
  
  TransformType::Pointer inputTransform = TransformType::New();
  
  TransformType::InputPointType inputPoint;
  TransformType::OutputPointType outputPoint;
  TempVolumeType::IndexType outputIndex;
  
  int targetIndex = 0;
  int inputIndex = 0;
  int maxTargetIndex = m_volumeSize[0]*m_volumeSize[1]*m_volumeSize[2];
  
  // Get raw data pointer
  unsigned char *inputPointer 
    = static_cast<unsigned char*>( usData->vtkData->GetScalarPointer() );
    

  /////////////////////////
  // 3D kernel resconstruction
  /////////////////////////
  
  typedef itk::Matrix< double, 3, 3 > MatrixType;
  MatrixType::InternalMatrixType splat_ui, splat_uv;
  int dx, dy, dz;
  int margin;
  
  // 1.54 => 50% conf. interval
  // 2.03 => 75% conf. interval
  // 2.5  => 90% conf. interval
  // 2.79 => 95% conf. interval
  // 3.36 => 99% conf. interval
  //double ellipsoid_scale_factor = 2.5;//2.03;
  
  // Kernel reconstruction needs a temporary volume
  vtkImageData* tempVolume = vtkImageData::New();    
  
  // Kernel reconstruction needs some more preparation
  if (type == kernel)
  {
    // Init temporary volume
    //tempVolume->Initialize();
    //tempVolume->SetScalarTypeToUnsignedChar();    
    //tempVolume->SetScalarTypeToUnsignedShort();   
    tempVolume->SetScalarTypeToDouble();
    tempVolume->SetNumberOfScalarComponents(1);
    tempVolume->SetExtent(0, m_volumeSize[0]-1, 
                          0, m_volumeSize[1]-1, 
                          0, m_volumeSize[2]-1);	
    tempVolume->AllocateScalars();
        
    //double sigm_us_x, sigm_us_y, sigm_us_z;
    //sigm_us_x = 0.5;
    //sigm_us_y = 0.25;
    //sigm_us_z = 0.25;
    
    //sigm_us_x = 0.5;
    //sigm_us_y = 0.15;
    //sigm_us_z = 0.077;
        
    // Should be set from input or probe parametres / scanner settings
    //sigm_us_x = 1.5;
    //sigm_us_y = 1.0;
    //sigm_us_z = 0.5;
    
        
    // Define axes of 3D splat function in image coordinate system
    splat_ui(0,0) = 1 / (usData->sigm_us_x * usData->sigm_us_x);
    splat_ui(1,1) = 1 / (usData->sigm_us_y * usData->sigm_us_y);
    splat_ui(2,2) = 1 / (usData->sigm_us_z * usData->sigm_us_z);
    splat_ui(0,1) = 0; splat_ui(0,2) = 0;
    splat_ui(1,0) = 0; splat_ui(1,2) = 0;
    splat_ui(2,0) = 0; splat_ui(2,1) = 0;
    
    double extent = usData->ellipsoidScale * usData->sigm_us_x;
    // Not sure if the margin should be used
    margin = int(ceil(max(max(extent / m_volumeSpacing[0], 
                              extent / m_volumeSpacing[1]),
                          extent / m_volumeSpacing[2])));
  }
  
  double* tempVolumePtr = static_cast<double*>(tempVolume->GetScalarPointer()); 
  
  // Should the voxels be initialized or are they 0?
  /*for (unsigned int p=0; 
       p < m_volumeSize[0] * m_volumeSize[1] * m_volumeSize[2]; 
       p++)
  {
    if (type == kernel)
    {
      targetVolumeDoublePtr[p] = 0;
      tempVolumePtr[p] = 0;
    } else
      targetVolumeCharPtr[p] = 0;
  }*/
  
  ///////////////////////////////////////////////////////////////////////////
  // Needed for comparison test
  // The comparison test assumes that the target volume is not aligned with 
  // an external volume (no -tr used)
  ///////////////////////////////////////////////////////////////////////////
  unsigned int middleRecord = 0;
  if (removeFromRecord == 0)
    middleRecord = usData->numberOfRecords/2;
  else
    middleRecord = removeFromRecord;
  std::cout << "middleRecord: " << middleRecord << std::endl;
  //std::vector< std::vector<unsigned int> > removeData;
  //removeData->at(record).at(beam).at(sample);
  //removeData->at(beam).at(sample);
    
    
  //std::cout << "records: " << usData->numberOfRecords
  //  << " beams: " << usData->beams
  //  << " samples: " << usData->samples
  //  << std::endl;
  //while(record < usData->numberOfDataRecords) //et
  while(record < usData->numberOfRecords) 
//  record = 4;
//  while (record < 5)
  //while (record > (usData->numberOfRecords - 2))
  //if ((record >= usData->numberOfRecords / 2 - 1) || (record <= usData->numberOfRecords / 2 + 1) )  
    if ((usData->randomNumbersPercentage == 100 && record == middleRecord) ||
        (usData->randomNumbersPercentage == 300 && 
         (record >= middleRecord - 1 && record <= middleRecord + 1)) ||
        (usData->randomNumbersPercentage == 500 && 
         (record >= middleRecord - 2 && record <= middleRecord + 2)) ||
        (usData->randomNumbersPercentage == 700 && 
         (record >= middleRecord - 3 && record <= middleRecord + 3))
        )
    {
      //std::cout << "Skip record: " << record << std::endl;
      record++;
    }
  else
  {
    int skipped = 0;
    
    // Old code - possible interpretation
    // uv = ultrasound volume
    // rf = reference frame
    // td = tracked ?frame?
    // ui = ultrasound image
    // uvMui = uvMrf      * rfMtd     * tdMui
    //       = move origo * input pos * calibration
    // Looks like the above is incorrect
    // Try this instead
    // move origo * calibration * input pos 
    
    
    //std::cout << "record: " << record << std::endl;
    // Find the slice transform
//    sliceTransform->SetParameters(m_targetVolumeTransform->GetParameters());
    sliceTransform->SetParameters(m_calibration->GetParameters());
    //std::cout << "sliceTransform before: " << sliceTransform << std::endl;
    //readParams(sliceParams, usData->finalPositions->at(record)); //et
    readParams(sliceParams, usData->positionVector.at(record));
    
    //std::cout << "sliceParams: " << sliceParams << std::endl;
    
    inputTransform->SetParameters(sliceParams);
    //std::cout << "inputTransform: " << inputTransform << std::endl;
    sliceTransform->Compose(inputTransform, 0);
    //sliceTransform->Compose(inputTransform, 1); // old reconstruction format
    //std::cout << "sliceTransform: " << sliceTransform << std::endl;
    
    // Rotate target volume to middle slice
    sliceTransform->Compose(m_targetVolumeTransform, 0);
    
    // Move origo - composed(0) with m_targetVolumeTransform
    //sliceTransform->Compose(m_moveOrigo, 0);
    //std::cout << "moved origo: " << sliceTransform << std::endl;
    
    
    // Kernel reconstruction
    double* V_splat_uv = NULL;
    if (type == kernel)
    {
      // Transfer splat axes to volume coordinate system by rotation matrix 
      MatrixType::InternalMatrixType uvRui;
      uvRui(0,0) = sliceTransform->GetParameters()[0];
      uvRui(1,0) = sliceTransform->GetParameters()[1];
      uvRui(2,0) = sliceTransform->GetParameters()[2];
      
      uvRui(0,1) = sliceTransform->GetParameters()[3];
      uvRui(1,1) = sliceTransform->GetParameters()[4];
      uvRui(2,1) = sliceTransform->GetParameters()[5];
      
      uvRui(0,2) = sliceTransform->GetParameters()[6];
      uvRui(1,2) = sliceTransform->GetParameters()[7];
      uvRui(2,2) = sliceTransform->GetParameters()[8];
      
      splat_uv = uvRui * splat_ui * uvRui.transpose();
      
      double a, b, c, d, e, f;
      a = splat_uv(0,0);
      b = splat_uv(1,1);
      c = splat_uv(2,2);
      d = splat_uv(0,1) + splat_uv(1,0);
      e = splat_uv(0,2) + splat_uv(2,0);
      f = splat_uv(1,2) + splat_uv(2,1);
      
      //  * 3.36  =>  99% conf. interval
      //  * 2     =>  +- interval
      dx = int( ceil((2*usData->ellipsoidScale/sqrt(a)) 
                     / m_volumeSpacing[0]) );
      dy = int( ceil((2*usData->ellipsoidScale/sqrt(b)) 
                     / m_volumeSpacing[1]) );
      dz = int( ceil((2*usData->ellipsoidScale/sqrt(c)) 
                     / m_volumeSpacing[2]) );
      /*dx = int( ceil((2*ellipsoid_scale_factor/sqrt(splat_uv(0,0))) 
                     / m_volumeSpacing[0]) );
      dy = int( ceil((2*ellipsoid_scale_factor/sqrt(splat_uv(1,1))) 
                     / m_volumeSpacing[1]) );
      dz = int( ceil((2*ellipsoid_scale_factor/sqrt(splat_uv(2,2))) 
                     / m_volumeSpacing[2]) );*/
      if( (dx % 2) == 0 )		// //  Make dx, dy, dz odd numbers
        dx = dx + 1;
      if( (dy % 2) == 0 )
        dy = dy + 1;
      if( (dz % 2) == 0 )
        dz = dz + 1;

      if (dz==1)
      {
        //std::cout << "Setting dz to 3 intead of 1" << std::endl;
        dz=3;
      }
      
      if (record == 0 || record == middleRecord)
			std::cout << "Splat in pixels for record: " << record << " dx, dy, dz: " 
        << dx << ", " << dy << ", "<< dz << std::endl;

    
    // Fill rectangular box with 3D gaussian data 
    // =>  ellipsoid with main axes = transferred axes.
    V_splat_uv = new double[dx*dy*dz];
    for( int iA = 0; iA < dx; iA++ )
      for( int iB = 0; iB < dy; iB++ )
        for( int iC = 0; iC < dz; iC++ )
        {
          //x[iA][iB][iC] = (iA-((dx+1)/2)) * rcSpacing[0];
          //y[iA][iB][iC] = (iB-((dy+1)/2)) * rcSpacing[1];
          //z[iA][iB][iC] = (iC-((dz+1)/2)) * rcSpacing[2];
          
          double x, y, z, expon;
          x = (iA-((dx+1)/2)) * m_volumeSpacing[0];
          y = (iB-((dy+1)/2)) * m_volumeSpacing[1];
          z = (iC-((dz+1)/2)) * m_volumeSpacing[2];
          
          expon = -(a*(x*x)) -(b*(y*y)) -(c*(z*z)) - (d*(x*y)) -(e*(x*z)) 
                  -(f*(y*z));
          V_splat_uv[iA*dy*dz + iB*dz + iC] = exp(expon);
          
        }
    
    }//kernel
    
    // Find minimum input value to use for empty voxels
    /*int tempIndex = 0;
    int inMin = USHRT_MAX;
    for (int tempBeam = 0; tempBeam < usData->beams; tempBeam++)
    {
      //std::cout << "inMin: " << inMin << std::endl;
      for (int tempSample = 0; tempSample < usData->samples; tempSample++)
      {
        tempIndex = tempBeam + tempSample * usData->beams 
        + record * usData->beams * usData->samples;
        if (inMin > inputPointer[tempIndex])
          if (inputPointer[tempIndex] > 0)
            inMin = inputPointer[tempIndex];
        //tempIndex += usData->beams;
      }
      //tempIndex++;
    }
    std::cout << "record: " << record << " inMin: " << inMin << std::endl;
    */
    beam = 0;
    while (beam < usData->beams)
    { 
      //std::cout << "beam: " << beam << std::endl;
    
      // TODO:
      // Move outside and find minimum for one slice or all slices, 
      // not just each beam?
      // -> No -> Looks like we get better results with just a local minimum,
      // because of the possibility to remove empty beams
      
      // Find minimum input value to use for empty voxels
      bool emptyBeam = false;
      int tempIndex = 0;
      int inMin = UCHAR_MAX;
      //int inMin = USHRT_MAX;
        //std::cout << "inMin: " << inMin << std::endl;
        for (unsigned int tempSample = 0; tempSample < usData->samples; tempSample++)
        {
          tempIndex = beam + tempSample * usData->beams 
          + record * usData->beams * usData->samples;
          if (inMin > inputPointer[tempIndex])
            if (inputPointer[tempIndex] > 0)
              inMin = inputPointer[tempIndex];
          //tempIndex += usData->beams;
          
          // Find max input value. Code may be removed if not needed
          if (inputMax < inputPointer[tempIndex])
            inputMax = inputPointer[tempIndex];
        }
        // Set InMin to zero if beam is empty
        if (inMin == USHRT_MAX)
        {
          inMin = 0;
          emptyBeam = true;
        }
        // Fint min input value. Code may be removed in not needed.
        if (inputMin > inMin)
          inputMin = inMin;
      //std::cout << "record: " << record << " inMin: " << inMin << std::endl;
        
      
      sample = 0;
      // Skip empty beams
      if (emptyBeam)
      {
        sample = usData->samples;
        std::cout << "Empty beam. Record: " << record << " beam: " << beam << std::endl;
      }
      
      if (usData->removeData.empty())
        throw("usData->removeData list empty");
      //if (usData->removeData[beam].empty())
      //  throw("usData->removeData vector empty");
      
      std::list<unsigned int>::iterator removeSampleIterator 
        = usData->removeData[beam].begin();
      while (sample < usData->samples)
      {
        //std::cout << "sample: " << sample << std::endl;
        
        
        ///////////////////////////////////////////////////////////////////////
        // Needed for comparison test
        bool dontRemove = true;
        // Remove data from middle record if random numbers is present
        if (record == middleRecord && (usData->randomNumbersPercentage !=0 ) && 
            removeSampleIterator != usData->removeData[beam].end())
        {
          if (sample == (*removeSampleIterator))
          {
            dontRemove = false;
            removeSampleIterator++;
          }
        }
        ///////////////////////////////////////////////////////////////////////
        
        //findInputPoint(et, inputPoint, et->polar);
        // May possibly speed up this by incremental additions
        if (usData->polar)
        {
          double depth = usData->startDepth + usData->depthIncrement * sample;
          double angle = usData->startAngle + usData->angleIncreament * beam;
          inputPoint[0] = 0;
          inputPoint[1] = depth * sin(angle);
          inputPoint[2] = depth * cos(angle);
        } else {  // Cartesian
                  // See also old code to do this correctly
          //inputPoint[0] = 0;
          //inputPoint[1] = (beam - usData->beams/2.0)  * m_volumeSpacing[1];
          //inputPoint[2] = sample * m_volumeSpacing[2];
          inputPoint[0] = 0;
          inputPoint[1] = (beam - usData->beams/2.0)  * usData->beamWith;
          inputPoint[2] = sample * usData->sampleDepth;
        }
        
          
        // Test with only input positions
        outputPoint = sliceTransform->TransformPoint(inputPoint);
        //outputPoint = inputTransform->TransformPoint(inputPoint);
        outputIndex[0] = (int)(outputPoint[0] / m_volumeSpacing[0]);
        outputIndex[1] = (int)(outputPoint[1] / m_volumeSpacing[1]);
        outputIndex[2] = (int)(outputPoint[2] / m_volumeSpacing[2]);
        
        // Input pixel
        //et->volume->at(record).at(beam).at(sample)
        
        targetIndex = outputIndex[0] + outputIndex[1]*m_volumeSize[0] + 
          outputIndex[2]*m_volumeSize[0]*m_volumeSize[1];
        
        //std::cout << "inputPoint: "<< inputPoint[0] << " " 
        //  << inputPoint[1] << " "<< inputPoint[2] <<  std::endl;
        
        //std::cout << "outputPoint: "<< outputPoint[0] << " " 
        //  << outputPoint[1] << " "<< outputPoint[2] <<  std::endl;
        
        //std::cout << "outputIndex: "<< outputIndex[0] << " " 
        //  << outputIndex[1] << " "<< outputIndex[2] <<  std::endl;
        //std::cout << "targetIndex: "<< targetIndex <<  std::endl;
                
        //if (targetIndex > 0 && targetIndex < maxTargetIndex)
        if (targetIndex > 0 && targetIndex < maxTargetIndex
          &&outputIndex[0] >= 0 && outputIndex[0] <= static_cast<long>(m_volumeSize[0])
          && outputIndex[1] >= 0 && outputIndex[1] <= static_cast<long>(m_volumeSize[1])
          && outputIndex[2] >= 0 && outputIndex[2] <= static_cast<long>(m_volumeSize[2]) 
          && dontRemove)
        {
          //inputIndex = sample + beam * usData->samples 
          //+ record * usData->beams * usData->samples;
          // Swapped samples and beams (because of the VTK volume)
          inputIndex = beam + sample * usData->beams 
            + record * usData->beams * usData->samples;
          //int altIndex = usData->vtkData->FindPoint(beam, record, sample);
          
          //std::cout << "  inputIndex: " << inputIndex << " altIndex: " 
          //  << altIndex << std::endl;
          
          //if (altIndex > 0)
          
          if (type == PNN)
          {
          //test
          /*targetVolumePtr = 
            static_cast<unsigned char*>(targetVolume->
                                        GetScalarPointer(outputIndex[0], 
                                                         outputIndex[1], 
                                                         outputIndex[2]));
          */
            // Use max values
            //if (targetVolumeCharPtr[targetIndex] < inputPointer[inputIndex])
            //{
              //std::cout << targetVolumeCharPtr[targetIndex] << std::endl;
            targetVolumeCharPtr[targetIndex] =
            //static_cast<double>(usData->volume->at(record).at(beam).at(sample));
            inputPointer[inputIndex];
            //inputPointer[altIndex];//test - use VTK function instead
            //}

          //std::cout << "  inputIndex: " << inputIndex << " targetIndex: " 
          //  << targetIndex << std::endl;
          //std::cout << "Didn't skip targetIndex: "<< targetIndex 
          //<< " record: " << record << std::endl;
          } else if (type == kernel)
          {
            
            // Modify targetIndex. 
            // Will be used to traverse a local neighborhood.
            //targetIndex = targetIndex -((dx-1)/2) - ((dy-1)/2*m_volumeSize[0])
            //              - ((dz-1)/2*m_volumeSize[0]*m_volumeSize[1]);
            /*Old code
            //from
            rx = Puv_ind[0]+margin+1-((dx-1)/2);
            ry = Puv_ind[1]+margin+1-((dy-1)/2);
            rz = Puv_ind[2]+margin+1-((dz-1)/2);
            
            //to
            toX = Puv_ind[0]+margin+((dx+1)/2);
            toY = Puv_ind[1]+margin+((dy+1)/2);
            toZ = Puv_ind[2]+margin+((dz+1)/2);*/
            
            // Insert found inMin value in empty voxels
            //if (targetVolumePtr[targetIndex] < inMin)
            //  targetVolumePtr[targetIndex] = inMin;
            
            // Traverse the local neighborhood
            /* Old code
            int index = 0;//rx + ry*outXdim + rz*outXdim*outYdim;
            for( int k=rz; k<=toZ; k++ )
              for( int j=ry; j<=toY; j++ )
                for( int i=rx; i<=toX; i++ )
            */
            //double weight, counter; 
            int splatxmax = (dx-1)/2;
            int splatymax = (dy-1)/2;
            int splatzmax = (dz-1)/2;
            //for ( int x = -(dx-1)/2; x < (dx-1)/2; x++ )
            //  for ( int y = -(dy-1)/2; y < (dy-1)/2; y++ )
            //    for ( int z = -(dz-1)/2; z < (dz-1)/2; z++ )
            for ( int x = -splatxmax; x < splatxmax; x++ )
              for ( int y = -splatymax; y < splatymax; y++ )
                for ( int z = -splatzmax; z < splatzmax; z++ )
                {
                  int newTargetIndex = targetIndex + x + y*m_volumeSize[0] +
                  z*m_volumeSize[0]*m_volumeSize[1];
                  
                  if (newTargetIndex > 0 && newTargetIndex < maxTargetIndex
                      && ((outputIndex[0]+x) >= 0) && ((outputIndex[0]+x) < static_cast<long>(m_volumeSize[0]))
                      && ((outputIndex[1]+y) >= 0) && ((outputIndex[1]+y) < static_cast<long>(m_volumeSize[1]))
                      && ((outputIndex[2]+z) >= 0) && ((outputIndex[2]+z) < static_cast<long>(m_volumeSize[2])))
                  {
                    // look at this code to speed up processing time
                    
                    // Insert found inMin value in empty voxels
                    if (targetVolumeDoublePtr[newTargetIndex] < inMin)
                      targetVolumeDoublePtr[newTargetIndex] = inMin;
                    
                    //double weight = V_splat_uv[x*dy*dz + y*dz +z];
                    //double weight = V_splat_uv[(x+(dx-1)/2)*dy*dz + (y+(dy-1)/2)*dz +z+(dz-1)/2];
                    double weight = V_splat_uv[(x+splatxmax)*dy*dz + (y+splatymax)*dz +z+splatzmax];
                    double counter = ( weight * inputPointer[inputIndex] ) + 
                      ( tempVolumePtr[newTargetIndex] * 
                        targetVolumeDoublePtr[newTargetIndex] );
                      
                    //std::cout << "xyz: " << x << " " << y << " " << z 
                    //<< " weight: " << weight << " counter: " << counter
                    //<< " V_splat_uv: " 
                    //<< V_splat_uv[(x+dx)*dy*dz + (y+dy)*dz +z+dz] << std::endl;
                    
                    tempVolumePtr[newTargetIndex] = 
                      weight + tempVolumePtr[newTargetIndex];
                    
                    targetVolumeDoublePtr[newTargetIndex] = 
                      counter / tempVolumePtr[newTargetIndex];
                  }
                }
                  
            
            //double weight = V_splat_uv[dy*dz*(i-rx) + dz*(j-ry)+ k-rz];
            
            //targetVolumePtr[targetIndex] = inputPointer[inputIndex];
            
          }//kernel
          
        }
        else
        {
          //std::cout << "Skipped targetIndex: "<< targetIndex << " record: " 
          //  << record << std::endl;
          skipped++;
        }
        
        sample++;
      }//sample
      
      beam++;
    }//beam
    
    //test
    //std::cout << "outputPoint: "<< outputPoint[0] << " " 
    //  << outputPoint[1] << " "<< outputPoint[2] <<  std::endl;
    //std::cout << "outputIndex: "<< outputIndex[0] << " " 
    //  << outputIndex[1] << " "<< outputIndex[2] <<  std::endl;
    
    //if ( skipped > 0 )
    //  std::cout << "record: " << record << " Skipped: "<< skipped << std::endl;
    
    record++;
  }//record
  
  //itk->vtk Image conversion (itkImageToVTKImageFilter)
  //test: removed ITK components
/*  typedef itk::RescaleIntensityImageFilter< TempVolumeType, FinalVolumeType > 
    InputTToCustusTFilterType;
  InputTToCustusTFilterType::Pointer iTCFilter = 
    InputTToCustusTFilterType::New();
  iTCFilter->SetInput(targetVolume);
	// Max practical is 12 bits = 16 bits right-shifted 4
  iTCFilter->SetOutputMaximum(USHRT_MAX >> 4); 
	iTCFilter->SetOutputMinimum(0);
	iTCFilter->Update();
    
	itk::ImageToVTKImageFilter<FinalVolumeType>::Pointer itk2vtk = 
    itk::ImageToVTKImageFilter<FinalVolumeType>::New();
	itk2vtk->SetInput(iTCFilter->GetOutput());
  
  // Make a "deep" copy of the itk pipeline output (i.e the resulting vtkImage)
  // for further use, as the itk pipeline is destroyed
  vtkImageData* vtkImg = vtkImageData::New();
  itk2vtk->Update();
  vtkImg->DeepCopy(itk2vtk->GetOutput());

  return(vtkImg);
  */
  
  
  t2 = clock();
  double dif = (t2 - t1);
  dif = dif / CLOCKS_PER_SEC;
  std::cout << "Total reconstruction time: " << dif << " sec" << std::endl;
  
/*  if (type == PNN)
  {
    if (interpolationSteps == 0)
      return(targetVolume);
    else
      return(interpolate(targetVolume, interpolationSteps));
  }*/
  //else //kernel
  //{
	vtkImageShiftScale *shiftvolume = vtkImageShiftScale::New();
	shiftvolume->SetShift(0);
  // Can't scale volume if we compare rms of input and output
  if (maxRange && (usData->randomNumbersPercentage == 0))
  {
    std::cout << "inputMin: " << inputMin << " inputMax: " << inputMax 
    << " scale: " << UCHAR_MAX / static_cast<double>(inputMax-inputMin) 
    << std::endl;
	  shiftvolume->SetScale(UCHAR_MAX / static_cast<double>(inputMax-inputMin));
	  //shiftvolume->SetScale(USHRT_MAX/(max-inMin));
  }
	shiftvolume->ClampOverflowOn();
	shiftvolume->SetOutputScalarTypeToUnsignedChar();
	shiftvolume->SetInput(targetVolume);
	shiftvolume->Update();
  
  vtkImageData* returnVolume = NULL;
  
  if (interpolationSteps == 0)
    //return(shiftvolume->GetOutput());
    returnVolume = shiftvolume->GetOutput();
  else
    //return(interpolate(shiftvolume->GetOutput(), interpolationSteps));
    returnVolume = interpolate(shiftvolume->GetOutput(), interpolationSteps);
  //}
  
  // Calculate RMS values for removed data
  
  //std::vector<std::list<unsigned int> >::iterator beamIter;
  std::list<unsigned int>::iterator sampleIter;
  
  
  //double *returnVolumeDoublePtr = 
  //static_cast<double*>(returnVolume->GetScalarPointer());
  unsigned char *returnVolumeCharPtr = 
  static_cast<unsigned char*>(returnVolume->GetScalarPointer());
  
  //for (beamIter = usData->removeData.begin();
  //     beamIter != usData->removeData.end(); beamIter++)
  unsigned int beams = usData->beams; 
  
  //std::cout << "RMS calculations" << std::endl;
  // RMS
  //if (usData->hasRandomNumbers)
  //{
    int totalSquaredError = 0;
    //int numberOfRemovedSamples = 0;
    
    /*unsigned int startRecord, stopRecord;
    if (usData->randomNumbersPercentage <= 100)
    {
      //std::cout << "randomNumbersPercentage <= 100 " << std::endl;
      startRecord = middleRecord;
      stopRecord = middleRecord;
    }
    else if (usData->randomNumbersPercentage == 300)
    {
      startRecord = middleRecord - 1;
      stopRecord = middleRecord + 1;
    }
    else if (usData->randomNumbersPercentage == 500)
    {
      startRecord = middleRecord - 2;
      stopRecord = middleRecord + 2;
    }
    else if (usData->randomNumbersPercentage == 700)
    {
      startRecord = middleRecord - 3;
      stopRecord = middleRecord + 3;
    }*/
  
    //std::cout << " beams: " << beams << " samples: " << usData->samples << std::endl;
    unsigned int skipped = 0;
    
  // Open file for writing all samples, 
  std::ofstream smplFileStream;
  std::stringstream smplFileName;
  smplFileName << outputFileName << "_" << middleRecord<< ".smpl";
  smplFileStream.open(smplFileName.str().c_str(), 
                  std::ios_base::out);
  if (smplFileStream.fail())
  {
    std::cout << "*** Error: Couldn't open file for writing: " 
    << smplFileName.str() << std::endl;
  }
  // Write heading
  //smplFileStream << "inPoint" << "\t" << "outPoint" << "\t" << "error" 
  //<< "\t" << "absValue" << std::endl;
  smplFileStream << "inPoint" << "\t" << "outPoint" << std::endl;
  
  record = middleRecord;
    //for (unsigned int record = startRecord; record <= stopRecord; record++)
    for (unsigned int beam = 0; beam < beams; beam++)
      //for (sampleIter = (*beamIter).begin();
      //     sampleIter != (*beamIter).end(); sampleIter++)
      for (unsigned int sample = 0; sample < usData->samples; sample++)
      //for (sampleIter = usData->removeData[beam].begin();
      //     sampleIter != usData->removeData[beam].end(); sampleIter++) 
      {
        //std::cout << "record: " << record;
        //std::cout << " beam: " << beam << " sample: " << sample << std::endl;
        /////
        // Copied code
        sliceTransform->SetParameters(m_calibration->GetParameters());
        readParams(sliceParams, usData->positionVector.at(record));
        inputTransform->SetParameters(sliceParams);
        sliceTransform->Compose(inputTransform, 0);
        sliceTransform->Compose(m_targetVolumeTransform, 0);
        
        inputPoint[0] = 0;
        inputPoint[1] = (beam - usData->beams/2.0)  * usData->beamWith;
        //inputPoint[2] = (*sampleIter) * usData->sampleDepth;
        inputPoint[2] = sample * usData->sampleDepth;
        
        outputPoint = sliceTransform->TransformPoint(inputPoint);
        outputIndex[0] = (int)(outputPoint[0] / m_volumeSpacing[0]);
        outputIndex[1] = (int)(outputPoint[1] / m_volumeSpacing[1]);
        outputIndex[2] = (int)(outputPoint[2] / m_volumeSpacing[2]);
        targetIndex = outputIndex[0] + outputIndex[1]*m_volumeSize[0] + 
        outputIndex[2]*m_volumeSize[0]*m_volumeSize[1];
        
        //inputIndex = beam + (*sampleIter) * usData->beams 
        inputIndex = beam + sample * usData->beams 
        + record * usData->beams * usData->samples;
        /////
        
        
        int error = 0;
        //TODO: Don't use values that are outside the volume
        
        if (targetIndex > 0 && targetIndex < maxTargetIndex
            && ((outputIndex[0]) >= 0) && ((outputIndex[0]) < static_cast<long>(m_volumeSize[0]))
            && ((outputIndex[1]) >= 0) && ((outputIndex[1]) < static_cast<long>(m_volumeSize[1]))
            && ((outputIndex[2]) >= 0) && ((outputIndex[2]) < static_cast<long>(m_volumeSize[2])))
        {
          //error = inputPointer[inputIndex] - returnVolumeCharPtr[targetIndex];
          int inPoint = inputPointer[inputIndex];
          int outPoint = returnVolumeCharPtr[targetIndex];
          error = inPoint - outPoint;
          //smplFileStream << inputPointer[inputIndex] << "\t"
          //<< returnVolumeCharPtr[targetIndex] << std::endl;
          //smplFileStream << inPoint << "\t" << outPoint << "\t" << error 
          //<< "\t" << sqrt(error*error) << std::endl;
          smplFileStream << inPoint << "\t" << outPoint << std::endl;
        }
        else
        {
          //std::cout << "Removed point outside target volume. ";
          //std::cout << "Skipped record: " << record;
          //std::cout << " beam: " << beam << " sample: " << sample << std::endl;
          skipped++;
          smplFileStream << std::endl; // skip point
        }
        
        totalSquaredError += error*error;
        //numberOfRemovedSamples++;
      }
    smplFileStream.close();
  
  
    int numberOfSamples = usData->beams * usData->samples;
    //double ms = totalSquaredError / double(numberOfRemovedSamples);
    double ms = totalSquaredError / double(numberOfSamples - skipped);
    double rms = sqrt(ms);
    
    //double percentage = numberOfRemovedSamples / double(numberOfSamples) * 100;
    
    if (skipped != 0)
    {
      std::cout << "Input points outside target volume are skipped ";
      std::cout << skipped << std::endl;
    }
  
    std::cout << "RMS: " << rms 
    //<< " Percentage removed: " << percentage 
    << std::endl;
    
    if (!usData->rmsFile.empty())
    {
      std::ofstream fileStream;
      fileStream.open(usData->rmsFile.c_str(), 
                      std::ios_base::out | std::ios_base::app);
      if (!fileStream.fail())
      {
        //fileStream << "Percentage\t" 
        fileStream << usData->randomNumbersPercentage 
        << "\t" 
        //<< "rms\t" 
        << rms << std::endl;      
      }
      fileStream.close();
    }
  //}
  
  return returnVolume;
}

vtkImageData* UsReconstruction::interpolate(vtkImageData* inputVolume,
                                            int interpolationSteps)
{
  //std::cout << "UsReconstruction::interpolate() called. interpolationSteps: "
  //<< interpolationSteps << std::endl;
    clock_t t1, t2;
    t1 = clock();
    
    //Init vtkImageData
    vtkImageData* targetVolume = vtkImageData::New();    
    targetVolume->SetScalarTypeToUnsignedChar();    
    //targetVolume->SetScalarTypeToUnsignedShort();   
    //targetVolume->SetScalarTypeToDouble();
    targetVolume->SetNumberOfScalarComponents(1);
    targetVolume->SetExtent(0, m_volumeSize[0]-1, 
                            0, m_volumeSize[1]-1, 
                            0, m_volumeSize[2]-1);	
    targetVolume->AllocateScalars();
    unsigned char *targetVolumePtr = 
      static_cast<unsigned char*>(targetVolume->GetScalarPointer());
    unsigned char *inputVolumePtr = 
      static_cast<unsigned char*>(inputVolume->GetScalarPointer());
    
    
		int index = 0;
    //int maxIndex = m_volumeSize[0]*m_volumeSize[1]*m_volumeSize[2];
		
		//std::cout << "m_volumeSize (x, y, z): " <<  m_volumeSize[0] <<  " " 
    //  <<  m_volumeSize[1] <<  " " <<  m_volumeSize[2] << std::endl;
		
    // Don't interpolate along the edges
    int margin = interpolationSteps;
    
		//for (int local_area = 1; local_area <= 2; local_area++)
		
		for (unsigned int x = margin; x < m_volumeSize[0] - margin; x++)
			for (unsigned int y = margin; y < m_volumeSize[1] - margin; y++)
				for (unsigned int z = margin; z < m_volumeSize[2] - margin; z++)
		//for (unsigned int x = 0; x < m_volumeSize[0]; x++)
		//	for (unsigned int y = 0; y < m_volumeSize[1]; y++)
		//		for (unsigned int z = 0; z < m_volumeSize[2]; z++)
				{
          index = x + y*m_volumeSize[0] + z*m_volumeSize[0]*m_volumeSize[1];
					//if (tmpAccArr[index] == 0 )
					//std::cout << "tmpAccArr[index]: " << tmpAccArr[index] << std::endl;
					//if ( tmpAccArr[index] > -0.001 && tmpAccArr[index] < 0.1 )
					if ( inputVolumePtr[index] < 0.1 )
					{						
            //std::cout << "x: " << x << " y: " << y 
            //<< " z: " << z << std::endl;
						int tmpindex = 0;
						int count = 0;
						double temp = 0;
						
						int local_area = 1;
						bool found = false;
						do
						{
							for( int k=-local_area; k<=local_area; k++ )
								for( int j=-local_area; j<=local_area; j++ )
									for( int i=-local_area; i<=local_area; i++ )
									{
										//tmpindex = index + i + j*outXdim + k*outXxYdim; 
                    tmpindex = index + i + j*m_volumeSize[0] + 
                      k*m_volumeSize[0]*m_volumeSize[1];
										//max value interpolation
                    //	if ( tmpAccArr[tmpindex] > 0.1 )
                    //	{
                    //		if (tmpAccArr[tmpindex] > temp)
                    //			temp = tmpAccArr[tmpindex];
                    //	}										
										
										// mean value interpolation
										if ( inputVolumePtr[tmpindex] > 0.1 )
										{
											temp += inputVolumePtr[tmpindex];
											count++;
										}
									}
                    if ( count > 0)
                    {
                      found = true;
                      // No round() on Windows
                      //targetVolumePtr[index] = round(temp / count); 
                      targetVolumePtr[index] = floor((temp / count) + 0.5);

                      //tmpAccArr[index] = temp / count;
                      //std::cout << "count: " << count << " inserted: " 
                      //  << targetVolumePtr[index] << std::endl;
                      //printf("count: %d inserted: %d\n", count, 
                      //       targetVolumePtr[index]);
                    }
                    
                    //		}                // max value interpolation
                    //tmpImgArr[index] = temp; // max value interpolation
                    
                    ++local_area;
          } while (local_area <= interpolationSteps && !found);
        }
  }
          
          // Insert calculated values in original volume
          for (unsigned int x = margin ; x < m_volumeSize[0] - margin; x++)
            for (unsigned int y = margin ; y < m_volumeSize[1] - margin; y++)
              for (unsigned int z = margin ; z < m_volumeSize[2] - margin; z++)
              {
                index = x + y*m_volumeSize[0] + 
                z*m_volumeSize[0]*m_volumeSize[1];
                //if (index > 0 && index < maxIndex && inputVolumePtr[index] > 0.1 )
                //  targetVolumePtr[index] = inputVolumePtr[index];
                //if (inputVolumePtr[index] > 0.1 )
                //  targetVolumePtr[index] = inputVolumePtr[index];
                if (targetVolumePtr[index] > 0.1 )
                  inputVolumePtr[index] = targetVolumePtr[index];
              }
                //std::cout << "Interpolation finished" << std::endl;
                
                
    t2 = clock();
    double dif = (t2 - t1);
    dif = dif / CLOCKS_PER_SEC;
    std::cout << "Total interpolation time: " << dif << " sec" << std::endl;
    
    //return(targetVolume);
    return(inputVolume);
}

// Should probably be in a volume class
void UsReconstruction::writeFile(vtkImageData* volume, std::string filename)
{
  // Use vtkMetaImageWriter
  
  vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
  //(writer)->SetInputConnection(volume->GetOutputPort());

  // Writer always writes 1 1 1 as spacing???
//  volume->SetSpacing(m_volumeSpacing[0], 
//                     m_volumeSpacing[1], 
//                     m_volumeSpacing[2]);
  //volume->SetSpacing(2.0,2.0,2.0);
  volume->Update();
  //std::cout << *volume << std::endl;
  std::cout << "Spacing: " << volume->GetSpacing()[0] << " " <<
  volume->GetSpacing()[1] << " " << volume->GetSpacing()[2] << std::endl;
  
  writer->SetInput(volume);
  writer->SetFileDimensionality(3);
  writer->SetFileName(std::string(filename + ".mdh").c_str());
  writer->SetRAWFileName(std::string(filename + ".raw").c_str());
  //Compression takes to much time
  writer->SetCompression(false);
  writer->Update();
  writer->Write();
  writer->Delete();
  
  /*
	//std::string mhaFilename = "test.mha";
  std::string mhaFilename = filename + ".mha";
  std::string rawFilename = filename + ".raw";
	//std::ofstream mhaFile(mhaFilename.c_str(), std::ios::out);
	FILE* mhaFile = fopen(mhaFilename.c_str(), "w");
	fprintf(mhaFile, "NDims = 3\n"
          "DimSize = %i %i %i\n"
          "ElementType = %s\n"
          "ElementSpacing = %f %f %f\n"
          "ElementByteOrderMSB = %s\n"
          "ElementDataFile = %s\n",
          m_volumeSize[0], m_volumeSize[1], m_volumeSize[2],
          //"MET_USHORT",//ITK
          "MET_UCHAR",//VTK
          m_volumeSpacing[0], m_volumeSpacing[1], m_volumeSpacing[2],
          //bigEndianFile_ ? "True" : "False",
          "False",
          rawFilename.c_str());
          //"test.raw");
	fclose(mhaFile);
	
	mhaFilename.clear();
  
  // Write .raw file
  vtkImageWriter* imgw = vtkImageWriter::New();
  //imgw->SetFileName(dirname_ + "/" + sfilename);
  //imgw->SetFileName("test.raw");
  imgw->SetFileName(rawFilename.c_str());
  imgw->SetInput(volume);
  imgw->SetFileDimensionality(3);
  imgw->Write();*/
}
void UsReconstruction::writeTransform(AffineTransformType::Pointer transform,
                                      TempVolumeType::SizeType volumeSize,
                                      std::string fileName)
{
  std::ofstream transformFileStream;
  transformFileStream.open(fileName.c_str());
  if (!transformFileStream.fail())
  {
    AffineTransformType::ParametersType position 
    = m_targetVolumeTransform->GetParameters();
    transformFileStream << volumeSize[0] << " " << volumeSize[1] << " "
    << volumeSize[2] << std::endl;
    transformFileStream << position[0] << " " << position[1] << " "
    << position[2] << " " << position[9]  << std::endl 
    << position[3] << " " << position[4]  << " " 
    << position[5] << " " << position[10] << std::endl 
    << position[6] << " " << position[7]  << " " 
    << position[8] << " " << position[11] << std::endl;
  }
  else
  {
    std::cout << "Can't open transform file for writing: " << fileName;
    std::cout << std::endl;
    throw ("Error writing transform file");
  }
  transformFileStream.close();
}

void UsReconstruction::readTransform(AffineTransformType::Pointer transform,
                                     //TempVolumeType::SizeType volumeSize,
                                     std::string fileName)
{
  //transform = AffineTransformType::New();
  AffineTransformType::ParametersType position 
  = transform->GetParameters();
  
  std::ifstream transformFileStream;
  transformFileStream.open(fileName.c_str());
  if (!transformFileStream.fail())
  {
    transformFileStream >> m_volumeSize[0] >> m_volumeSize[1] >> m_volumeSize[2];
    transformFileStream >> position[0] >> position[1] >> position[2] 
    >> position[9]  >> position[3] >> position[4] >> position[5] 
    >> position[10] >> position[6] >> position[7] >> position[8] 
    >> position[11];
  }
  else
  {
    std::cout << "Error opening transform file for reading: " << fileName;
    std::cout << std::endl;
    throw ("Error reading transform file");
  }
  transformFileStream.close();
  
  transform->SetParameters(position);
  
  //std::cout << "Volumesize read: " << m_volumeSize[0] << " " << m_volumeSize[1] 
  //<< " " << m_volumeSize[2] << std::endl;

}

void UsReconstruction::findRandomNumbers(Input2DUS* usData,
                                         std::string fileName)
{
  std::cout << "UsReconstruction::findRandomNumbers called" << std::endl;
  if (usData->hasRandomNumbers)
    throw("Random numbers already exists");
      
  //std::cout << "RAND_MAX: " << RAND_MAX << std::endl;
  srand(time(NULL));
  
  unsigned int beams = usData->beams; 
  unsigned int samples = usData->samples;
  
  unsigned int repetitions = beams * samples * usData->randomNumbersPercentage / 100;

  
  for (unsigned int i = 0; i < beams; i++)
  {
    std::list<unsigned int> beamData;
    usData->removeData.push_back(beamData);
  }
  
  //usData->hasRandomNumbers = true;
  
  // Don't generate random values if percentage >= 100
  // as whole slices are removed
  if (usData->randomNumbersPercentage >= 100)
    //return;
    repetitions = 0;
  
  /*for (unsigned int i = 0; i < repetitions; i++)
  {
    unsigned int beam = random(beams);
    unsigned int sample = random(samples);
    usData->removeData[beam].push_back(sample);
  }*/
  //for (unsigned int beam = 0; beam < beams; beam++)
  //{
  
  std::list<unsigned int>::iterator sampleIter;
  
  unsigned int randomNumbers = 0;
  while (randomNumbers < repetitions)
  {
    unsigned int beam = random(beams);
    unsigned int sample = random(samples);
    sampleIter = usData->removeData[beam].begin();
    while ( sampleIter != usData->removeData[beam].end() && 
           (*sampleIter) < sample)
      sampleIter++;
    if ((*sampleIter) != sample)
    {
      usData->removeData[beam].insert(sampleIter, sample);
      randomNumbers++;
    }
  }
  
  // Sort and remove duplicates
  /*std::vector<std::list<unsigned int> >::iterator beamIter;
  for (beamIter = usData->removeData.begin();
       beamIter != usData->removeData.end(); beamIter++)
  {
    //std::sort(usData->removeData[beam].begin(), usData->removeData[beam].end());
    (*beamIter).sort();
    // Remove duplicates
    // TODO: Should they be counted and an equal number of new values added?
    (*beamIter).unique();
  }*/

  // Only write file if precentage != 0
  //if (usData->randomNumbersPercentage != 0)
  //{
    std::ofstream fileStream;
    fileStream.open(fileName.c_str());
    if (!fileStream.fail())
    {
      for (unsigned int beam = 0; beam < beams; beam++)
      {
        fileStream << beam;
        for (std::list<unsigned int>::iterator iter = usData->removeData[beam].begin();
             iter != usData->removeData[beam].end(); iter++)
        {
          fileStream << " " << (*iter);
        }
        fileStream << std::endl;      
      }
    }
    else
    {
      std::cout << "Can't open random number file for writing: " << fileName;
      std::cout << std::endl;
      throw ("Error writing random number file");
    }
    fileStream.close();
    std::cout << "Random number file written: " << fileName << std::endl;
  //}
  usData->hasRandomNumbers = true;
}

unsigned int UsReconstruction::random(unsigned int max)
{
  
  unsigned int randInvRange = (RAND_MAX + 1) / max;
  unsigned int r;
  do
  {
    r = rand();
  } while (r >= max * randInvRange);
  r /= randInvRange;
  return r;
}
