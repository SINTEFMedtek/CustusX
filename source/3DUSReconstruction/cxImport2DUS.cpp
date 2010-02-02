//===========================================================================
//
// File: cxImport2DUS.cpp
//
// Created: 09.05.2008
//
//
/// \file
///       Reader for 2D ultrasound data with positions.
/// \author Ole Vegard Solberg
//===========================================================================

#include "cxImport2DUS.h"

#include <vtkMetaImageReader.h>
//#include <qstring.h>

#include <iostream>
#include <string>

Import2DUS::Import2DUS()
{
  usData = new Input2DUS();
}
Import2DUS::~Import2DUS()
{}



// Should use existing mhd reader and just import the data from here?
void Import2DUS::importmhd( const char *filename, 
                           bool oldDataFormat, 
                           double timeCalibration)
{
  //usData = inputData;
  
  //printf("inputfile \n ");
  readInputFile(filename);
  
  if (!oldDataFormat)
  {
    std::stringstream positionNameStream;
    positionNameStream << mFileName << "_timestamps.txt";
    readTimeTags(&usData->mTimestamps, positionNameStream.str());
  }
  
  //printf("posfile \n ");
  //readPosStackFile(mFileName);

  if (oldDataFormat)
    readPosStackFile("", oldDataFormat, timeCalibration);
  else
    readPosStackFile("1positions", oldDataFormat, timeCalibration);
    
  //printf("calfile \n ");
  
//  std::string calFileName = this->mFileName;
//  calFileName = calFileName + ".cal";
//  readCalFile(usData->calibration, calFileName);
    readCalFile();
  
  //Create two arrays that holds start and end points for the sector for each y-value
  //usData->sectorBeginArray = new int[usData->samples];
  //usData->sectorEndArray = new int[usData->samples];
  
  //initMaskArrays(usData->sectorBeginArray, usData->sectorEndArray);
  
  //std::stringstream fileNameStream;
  //fileNameStream << filename;
  //int number = 0;
  //fileNameStream >> number;
//  std::stringstream positionNameStream;
  //if (fileNameStream.fail()) // No number at start of filename
  //  positionNameStream << filename << "_timestamps";
  //else
  //  positionNameStream << number << filename << "_timestamps";
//  positionNameStream << mFileName << "_timestamps";
  
//  readTimeTags(&usData->mTimestamps, positionNameStream.str());
  //std::cout << "Files imported" << std::endl;
  //printf("initRcImage \n ");
  //  initRcImage();
}


// Use the VTK reader instead of ITK?

// filename should be without ending
void Import2DUS::readInputFile(const char *filename)
{
  this->mFileName = filename;
  // Remove file ending
  unsigned int loc = mFileName.rfind(".mha");
  if ( loc == std::string::npos )
    loc = mFileName.rfind(".mhd");
  if ( loc != std::string::npos )
    this->mFileName.erase(loc);
  else
    throw ("No meta header file (Must be .mha or .mhd)");
  
  // VTK
  // After a read - write sequence, the volume is mirrored
  
  // Use filename with ending
  vtkMetaImageReader *vtkReader = vtkMetaImageReader::New();
  std::string mhdFileName = filename;
  mhdFileName = mFileName + ".mhd";
    
  vtkReader->SetFileName( mhdFileName.c_str() );
  vtkReader->Update();
  
  
  // Moved to input2DUS
  //vtkImageData * vtkData = vtkImageData::New();
  
  // Deleting reader breaks pipe. Use DeepCopy
  //usData->vtkData = vtkReader->GetOutput();
  usData->vtkData->DeepCopy(vtkReader->GetOutput());
  
  usData->numberOfRecords = usData->vtkData->GetDimensions()[2];
  usData->samples         = usData->vtkData->GetDimensions()[1];
  usData->beams           = usData->vtkData->GetDimensions()[0];
  
//  std::cout << "numberOfRecords: " << usData->numberOfRecords << " samples: " 
//    << usData->samples << " beams: " << usData->beams << std::endl;
  
  // Use spacing
  usData->beamWith    = usData->vtkData->GetSpacing()[0];
  usData->sampleDepth = usData->vtkData->GetSpacing()[1];
  //std::cout << "spacing: " << usData->vtkData->GetSpacing()[0] << " " 
  //  << usData->vtkData->GetSpacing()[1] << " " 
  //  << usData->vtkData->GetSpacing()[2] << std::endl;
    
  vtkReader->Delete();
  
  // Read offset from .mhd file
  std::ifstream mhdFileStream;
  mhdFileStream.open(mhdFileName.c_str());
  bool offsetFound = false;
  if (!mhdFileStream.fail())
  {
    std::string inString;
    while (!mhdFileStream.eof() && !offsetFound)
    {
      mhdFileStream >> inString;
      //std::cout << inString << std::endl;
      if (inString == "Offset")
      {
        offsetFound = true;
        std::string temp;
        mhdFileStream >> temp; // Equal sign
        //int offset[3];
        mhdFileStream >> usData->mOffset[0] >> usData->mOffset[1];
        mhdFileStream >> usData->mOffset[2];
        std::cout << "Offset: " << usData->mOffset[0] << " "
        << usData->mOffset[1] << " " << usData->mOffset[2] << std::endl;
      }
    }
  }
  else
  {
    std::cout << "Error opening .mhd file for reading: " << mhdFileName;
    std::cout << std::endl;
    throw ("Error reading .mhd file");
  }
  mhdFileStream.close();
  
  // ITK
  /*
	typedef unsigned char PixelType;
  typedef itk::Image< PixelType, 3 > ImageType;
  typedef itk::ImageFileReader< ImageType >  ImageReaderType;
  
  ImageReaderType::Pointer reader;
  
  reader = ImageReaderType::New();
  reader->SetFileName( QString(this->filename + ".mhd").latin1() );
  reader->Update();  // Nødvendig for å få riktige verdier

	ImageType::Pointer fsImage = reader->GetOutput();
  
  
  // Dimension
  // std::cout << "Dimension: " << fsImage->GetImageDimension() << std::endl;
  // Region
  ImageType::IndexType	inputIndex;
  ImageType::SizeType	inputSize;
  ImageType::RegionType	inputRegion;
  
  inputRegion = fsImage->GetLargestPossibleRegion(); 
  inputIndex = inputRegion.GetIndex();
  inputSize = inputRegion.GetSize();
  //int xDimFS, yDimFS, zDimFS;
  //xDimFS =  inputRegion.GetSize()[2];    //NB
  //yDimFS =  inputRegion.GetSize()[0];
  //zDimFS =  inputRegion.GetSize()[1];
  
  usData->numberOfRecords = inputRegion.GetSize()[2];
  usData->samples         = inputRegion.GetSize()[1];
  usData->beams           = inputRegion.GetSize()[0];
  
  inputIndex = fsImage->GetBufferedRegion().GetIndex();
  inputSize = fsImage->GetBufferedRegion().GetSize();
  
  inputIndex = fsImage->GetRequestedRegion().GetIndex();
  inputSize = fsImage->GetRequestedRegion().GetSize();
  
  // Spacing
  //    const double * inputSpacing = fsImage->GetSpacing();
  //xSpaFS =  fsImage->GetSpacing()[2];      //NB
  //ySpaFS =  fsImage->GetSpacing()[0];
  //zSpaFS =  fsImage->GetSpacing()[1];
  
  std::cout << "xDimFS: " << xDimFS << " yDimFS: " << yDimFS 
    << " zDimFS: " << zDimFS << std::endl;
  std::cout << "xSpaFS: " << xSpaFS << " ySpaFS: " << ySpaFS 
    << " zSpaFS: " << zSpaFS << std::endl;
  
  
  //copy the raw data
  // memcpy?
  // Gives a pointer to the data
  // GetBufferPointer() returns PixelType
  // Assign memory ro usData first
  memcpy(usData->data, fsImage->GetBufferPointer(), 
         usData->numberOfRecords * usData->samples * usData->beams * 
         sizeof(PixelType));
  
  antFrames = xDimFS;// Set number of frames
    
  */  
}

void Import2DUS::readPosStackFile(std::string fileName, 
                                  bool oldDataFormat,
                                  double timeCalibration)
{  
  //std::cout << "Import2DUS::readPosStackFile() called" << std::endl;
  //typedef itk::AffineTransform<double, 3>	AffineTransformType;    
  AffineTransformType::Pointer positionTransform = AffineTransformType::New();
  //AffineTransformType::ParametersType rfMtd_Params = rfMtd->GetParameters();
  AffineTransformType::ParametersType position 
    = positionTransform->GetParameters();
  
  //move to header?
	//typedef std::vector<AffineTransformType::ParametersType> PositionVectorType;
    
  PositionVectorType positionVector;
  //reserve some positions, push_back will initalize memory positions
  positionVector.reserve(200);
  // Use push_back() to allocate more memory dynamically
	//positionVector.resize(10000); 
	
  int numPos = 0;
    
  std::ifstream posStackFileStream;
  std::stringstream posFileName, timeFileName;
  if (fileName.empty())
    posFileName << this->mFileName << ".pos";
  else
  {
    posFileName << fileName << "_transforms.txt";
    timeFileName << fileName << "_timestamps.txt";
    // Read time tags
    readTimeTags(&usData->mPosTimestamps, timeFileName.str());
  }
  
  posStackFileStream.open( posFileName.str().c_str() );
    
  if (!posStackFileStream.fail())
  {
    if (!oldDataFormat)
    {
      int numberOfPositions = 0;
      posStackFileStream >> numberOfPositions;
    }
    
    posStackFileStream >> position[0] >> position[1] >> position[2]
    >> position[9]   >> position[3] >> position[4] >> position[5] 
    >> position[10]  >> position[6] >> position[7] >> position[8] 
    >> position[11];
    
    numPos++;
    //positionVector[numPos++] = position;
    positionVector.push_back(position);
    
    while( !posStackFileStream.eof() )
    {
      posStackFileStream >> position[0] >> position[1] >> position[2] 
      >> position[9]     >> position[3] >> position[4] >> position[5] 
      >> position[10]    >> position[6] >> position[7] >> position[8] 
      >> position[11];
      
      numPos++;
      //positionVector[numPos++] = position;
      positionVector.push_back(position);
    }
  }
  else
  {
    std::cout << "Can't read position file: " << posFileName.str() 
    << std::endl;
    throw "Import2DUS::readPosStackFile: Image positions missing";  
  }
  posStackFileStream.close();
  
#ifndef RELEASE_MODE
	//std::cout << "numPos: " << numPos << std::endl;
#endif
	
  if (oldDataFormat)
    syncronize(numPos, positionVector);
  else
  {
    // syncronize only works when we have all time tags
    syncronize(positionVector, timeCalibration);
  }
  //antFrames = numPos; // Only use this value when reading from file
  
  //std::cout << "Import2DUS::readPosStackFile() finished" << std::endl;
}

void Import2DUS::readTimeTags(std::vector<double> *timeVector,
                              std::string fileName)
{
  //std::cout << "Import2DUS::readTimeTags called" << std::endl;
  std::stringstream timestampsName;
  if (fileName.empty())
    timestampsName << this->mFileName << "_timestamps.txt";
  else
    timestampsName << fileName;
  
  std::ifstream timestampsFileStream;
  timestampsFileStream.open(timestampsName.str().c_str());
  if (!timestampsFileStream.fail())
  {
    int numberOfTimestamps;
    timestampsFileStream >> numberOfTimestamps;
    
    timeVector->clear();
    double timestamp = 0;
    //std::cout << "numberOfTimestamps: " << numberOfTimestamps << std::endl;
    for (int i = 0; i < numberOfTimestamps; i++)
    {
      //std::cout << "i: " << i << std::endl;
      timestampsFileStream >> timestamp;
      timeVector->push_back(timestamp);
    }
    //std::cout << "for loop finished " << std::endl;
  }
  else
  {
    std::cout << "Can't read timetag file: " << timestampsName.str() 
    << std::endl;
  }
  timestampsFileStream.close();
  
  //std::cout << "Import2DUS::readTimeTags finished" << std::endl; 
}

void Import2DUS::syncronize(int numPositions, 
                            PositionVectorType& oldPositionVector)
{
#ifndef RELEASE_MODE
  cout << "USReconstruction::SYNCRONIZE usData->numberOfRecords: " 
  << usData->numberOfRecords << " numPos: " << numPositions << " " 
  << std::endl;
#endif
  
	if (numPositions == -1)
    //numPositions = fsImage->GetLargestPossibleRegion().GetSize()[2]+1;
    numPositions = usData->numberOfRecords;
  
  AffineTransformType::Pointer positionTransform = AffineTransformType::New();
  AffineTransformType::ParametersType rfMtd_Params 
    = positionTransform->GetParameters();
  
  //positionTransform->SetParameters( oldPositionVector[numPositions-1] );
	
  //========== Syncronize image and pos data ==================================
  //std::vector<AffineTransformType::ParametersType> rfMtd_ParamsFrams(300);
  double fac;
  
  
  // Moved to usData
  //PositionVectorType positionVector; // = rfMtd_ParamsFrams
  usData->positionVector.reserve(usData->numberOfRecords);
  
  //rfMtd_ParamsFrams.resize/*reserve*/(usData->numberOfRecords);
  fac = (double)numPositions / usData->numberOfRecords;
  //CX_printf("Syncronize factor : %f\n",fac);
  
  for( unsigned int i = 0; i < usData->numberOfRecords; i++ )
  {
    //rfMtd_ParamsFrams[i] = oldPositionVector[ int(i * fac) ];
    usData->positionVector.push_back(oldPositionVector[ int(i * fac) ]);
    
    //std::cout << "positionVector " << i << " " << positionVector[i] << std::endl; 
  }
}

void Import2DUS::syncronize(PositionVectorType& oldPositionVector, 
                            double timeCal)
{
  // ToDo: Use time calibration between images and positions
  // ToDo: Interpolate between positions
  
  if (usData->mTimestamps.empty())
    throw "Import2DUS::syncronize: Image time tags missing";  
  if (usData->mPosTimestamps.empty())
    throw "Import2DUS::syncronize: Position time tags missing";
  
  if (usData->numberOfRecords != usData->mTimestamps.size())
  {
    std::cout << "Number of images: " << usData->numberOfRecords 
    << " number of time tags: " << usData->mTimestamps.size() << std::endl;
    throw "Import2DUS::syncronize: Number of images and number of time tags don't match";
  }
  
  // Simpe time calibration uses only first time tag in each vector
  if ( (timeCal < 0.0000000001) && (timeCal > -0.0000000001) )
  {
    std::cout << "No time calibration, using first time tag in each vector";
    std::cout << std::endl;
    //timeCal = usData->mPosTimestamps[0] - usData->mTimestamps[0];
    timeCal = usData->mTimestamps[0] - usData->mPosTimestamps[0];
  }
  
  std::cout.precision(18);
//  std::cout << "timeCal: " << timeCal << std::endl;
  std::cout.precision(6);
  // Assumes time calibrated time tags and time tags of the same type
  // Place the image time tag between two position time tags and use the closest
  for (unsigned int i = 0; i < usData->numberOfRecords; i++ )
  {
    //std::cout << "record: " << i << std::endl;
    bool first = false;
    unsigned int j = 0;
    while (((usData->mTimestamps[i] * 1000) > (usData->mPosTimestamps[j] + timeCal)) 
      && (j < usData->mPosTimestamps.size()) )
    {
      //std::cout << "timstamp " << (usData->mTimestamps[i] - usData->mTimestamps[0]) << " < ";
      //std::cout << (usData->mPosTimestamps[j] + timeCal - usData->mTimestamps[0]) << std::endl;
      j++;
    }
    
    if (j+1 > usData->mPosTimestamps.size())
    {
      usData->positionVector.push_back(oldPositionVector[j]);
      std::cout << "Import2DUS::syncronize: No positions for images after: " 
      << i << std::endl;
      first = true;
      //return;
    }
    if (j == 0)
    {
      usData->positionVector.push_back(oldPositionVector[j]);
      std::cout << "Import2DUS::syncronize: No positions for first image"
      << std::endl;
      return;
    }
    
    //matrixInterpolation();
    
    //std::cout.precision(18);
    //std::cout << "image: " << i << " position: " << j;
    //std::cout << " image time: " << usData->mTimestamps[i] * 1000 << " pos time: ";
    //std::cout << usData->mPosTimestamps[j] + timeCal << std::endl;
    //std::cout.precision(6);
    if (first)
    {
      usData->positionVector.push_back(oldPositionVector[0]);
    }
    else if (((usData->mTimestamps[i] * 1000) <= (usData->mPosTimestamps[j] + timeCal)) &&
        ((usData->mTimestamps[i] * 1000) > (usData->mPosTimestamps[j-1] + timeCal)))
      // Choose the closest time tag
      if (((usData->mTimestamps[i] * 1000) - (usData->mPosTimestamps[j] + timeCal)) < 
          ((usData->mPosTimestamps[j-1] + timeCal) - (usData->mTimestamps[i] * 1000)))
        usData->positionVector.push_back(oldPositionVector[j]);
      else
        usData->positionVector.push_back(oldPositionVector[j-1]);
    else
      throw "Import2DUS::syncronize: Time tag error!";
  }
}

//void Import2DUS::readCalFile(AffineTransformType::Pointer calibration,
//                             std::string fileName)
void Import2DUS::readCalFile()
{
  
  //========== Read calFile ===================================================
  // std::cout << "=== calFile..." << std::endl;
  
//  /*AffineTransformType::Pointer */tdMui = AffineTransformType::New();
//  AffineTransformType::ParametersType tdMui_Params( tdMui->GetNumberOfParameters() );
  
  
  AffineTransformType::Pointer positionTransform = AffineTransformType::New();
  AffineTransformType::ParametersType position 
  = positionTransform->GetParameters();
  
  std::ifstream calFileStream;
  std::string fileName = this->mFileName;
  fileName = fileName + ".cal";
  calFileStream.open( fileName.c_str() );
  if (!calFileStream.fail())
  {
    calFileStream     >> position[0] >> position[1] >> position[2] 
      >> position[9]  >> position[3] >> position[4] >> position[5] 
      >> position[10] >> position[6] >> position[7] >> position[8] 
      >> position[11];
  }
  else // Open failed -> File is probably missing;
  {
    std::cout << "Can't read calibration file: " << fileName 
    << " Using identity matrix" << std::endl;
    position[0] = 1; position[1] = 0; position[2] = 0;
    position[3] = 0; position[4] = 1; position[5] = 0;
    position[6] = 0; position[7] = 0; position[8] = 1;
    position[9] = 0; position[10] = 0; position[11] = 0;
  }
  calFileStream.close();
  
 
  positionTransform->SetParameters( position );
  // Must be saved if it is going to be used
  usData->calibration = positionTransform;
  //calibration = positionTransform;
  //std::cout << positionTransform << std::endl;
}

vtkImageData* Import2DUS::getOutput()
{
  return(usData->vtkData);
}

Input2DUS* Import2DUS::getUsData()
{
  return(usData);
}

//! Operation: Interpolate transformation matrices.
// Suppose that T(t) is a time-variant 4-by-4 transformation matrix
// whos values at certain points, given in the vector DataPoints, are
// given in the vector DataValues. This operation, then, interpolates
// to find the values of T at the intermediate points given in the
// vector InterpolationPoints.
//
// The interpolated values are found in two different ways depending
// on the value of the InterpolationMethod string:
//   1. InterpolationMethod == "nearest neighbor": In this case, the
//      value at a given point is set to the value at the nearest
//      neighboring point with a known value.
//   2. InterpolationMethod == "linear": In this case, the rotation
//      part of the transformation matrix is found using the slerp
//      (spherical linear interpolation), while the translation part
//      is found using ordinary linear interpolation.
//
// Trondheim, 12.01.09.
// Lars Eirik B¯  <larseirik.bo@sintef.no>, SINTEF Technology and Society.

/*std::vector<vnl_matrix<double> > Import2DUS::matrixInterpolation(
                                                                       vnl_vector<double> DataPoints,
                                                                       std::vector<vnl_matrix<double> > DataValues,
                                                                       vnl_vector<double> InterpolationPoints,
                                                                       std::string InterpolationMethod )
{
  try
  {
    if ( DataPoints.size() != DataValues.size() )
    {
      std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
      //      << ">>>>>>>> In " << mstrScope 
      << "::Number of input data points differs from number of input data values!!!  Throw up ...\n";
      throw;
		}
    
    std::vector<vnl_matrix<double> > InterpolationData( InterpolationPoints.size() );
    
    if ( InterpolationMethod.compare("closest point") == 0 ) // Closest point "interpolation"
    {
      
      int i = 1;
      for ( int j = 0; j < InterpolationPoints.size(); j++ )
	    {
	      while ( DataPoints[i] < InterpolationPoints[j] && i < DataPoints.size() - 1 )
        {
          i++;
        }
	      
	      if ( std::abs( DataPoints[i-1] - InterpolationPoints[j] ) < std::abs( DataPoints[i] - InterpolationPoints[j] ) ) 
        {
          InterpolationData[j] = DataValues[i-1];
        }
	      else
        {
          InterpolationData[j] = DataValues[i];
        }
	    }
      
    }
    
    else if ( InterpolationMethod.compare("linear") == 0 ) // Linear interpolation
    {
      
      int j = 0;
      for ( int i = 0; i < InterpolationPoints.size(); i++ )
	    {
	      while ( DataPoints.get(j+1) < InterpolationPoints.get(i) && j+1 < DataPoints.size() - 1 )
        {
          j++;
        }
	      double t = ( InterpolationPoints.get(i) - DataPoints.get(j) )/( DataPoints.get(j+1) - DataPoints.get(j) );
	      
	      // Translation component interpolation
	      // -----------------------------------------------
        
	      vnl_vector<double> InterpolatedTranslationComponent(4);
	      for ( int k = 0; k < 3; k++ )
        {
          InterpolatedTranslationComponent.put( k, (1-t)*DataValues.at(j).get(k,3) + t*DataValues.at(j+1).get(k,3) );
        }
	      InterpolatedTranslationComponent.put( 3, 1 );
        
	      // Rotation matrix interpolation
	      // -----------------------------------------------
	      // This procedure is taken from Eberly (2008), "Rotation
	      // Representations and Performance Issues" found at
	      // http://www.geometrictools.com/.
        
	      // Step 1. Extract the rotational parts of the
	      //         transformation matrix and compute a matrix R.
        
	      vnl_matrix<double> P = DataValues.at(j).extract(3,3);
	      vnl_matrix<double> Q = DataValues.at(j+1).extract(3,3);
	      vnl_matrix<double> R = P.transpose()*Q;
	      
	      // Step 2. Compute an axis-angle representation of R.
        
	      vnl_vector<double> A( 3, 0 );
	      double Argument = ( R.get(0,0) + R.get(1,1) + R.get(2,2) - 1 )/2;
	      // Due to roundoff error, the argument can become
	      // slightly larger than 1, causing an invalid input to
	      // acos. In these cases, it is assumed that the rotation
	      // is negligable, and the argument is set to 1 (making
	      // theta 0).
	      if ( Argument > 1 )
        {
          Argument = 1;
        }
	      double theta = acos( Argument );
        
	      if ( theta == 0 )
        {
          // There is no rotation, and the vector is set to an
          // arbitrary unit vector ([1 0 0]).
          A.put( 0, 1 );
        }
	      else if ( theta < 3.14159265 )
        {
          A.put( 0, R.get(2,1) - R.get(1,2) );
          A.put( 1, R.get(0,2) - R.get(2,0) );
          A.put( 2, R.get(1,0) - R.get(0,1) );
          A.normalize();
        }
	      else
        {
          if ( R.get(0,0) > R.get(1,1) && R.get(0,0) > R.get(2,2) )
          {
            A.put( 0, sqrt( R.get(0,0) - R.get(1,1) - R.get(2,2) + 1 )/2 );
            A.put( 1, R.get(0,1)/( 2*A.get(0) ) );
            A.put( 2, R.get(0,2)/( 2*A.get(0) ) );
          }
          else if ( R.get(1,1) > R.get(0,0) && R.get(0,0) > R.get(2,2) )
          {
            A.put( 1, sqrt( R.get(1,1) - R.get(0,0) - R.get(2,2) + 1 )/2 );
            A.put( 0, R.get(0,1)/( 2*A.get(1) ) );
            A.put( 2, R.get(1,2)/( 2*A.get(1) ) );
          }
          else
          {
            A.put( 2, sqrt( R.get(2,2) - R.get(1,1) - R.get(0,0) + 1 )/2 );
            A.put( 1, R.get(1,2)/( 2*A.get(2) ) );
            A.put( 0, R.get(0,2)/( 2*A.get(2) ) );
          }
        }
        
	      // Step 3. Multiply the angle theta by t and convert
	      //         back to rotation matrix representation.
        
	      vnl_matrix<double> S( 3, 3, 0 );
	      vnl_matrix<double> I( 3, 3 );
	      vnl_matrix<double> Rt( 3, 3 );
	      
	      S.put( 0, 1, -A.get(2) );
	      S.put( 0, 2,  A.get(1) );
	      S.put( 1, 0,  A.get(2) );
	      S.put( 1, 2, -A.get(0) );
	      S.put( 2, 0, -A.get(1) );
	      S.put( 2, 1,  A.get(0) );
        
	      I.set_identity();
        
	      Rt = I + sin( t*theta )*S + ( 1 - cos( t*theta ) )*S*S;
        
	      // Step 4. Compute the interpolated matrix, known as the
	      //         slerp (spherical linear interpolation).
        
	      vnl_matrix<double> InterpolatedRotationComponent = P*Rt;
        
	      // Transformation matrix composition
	      // -----------------------------------------------
	      // Compose a 4x4 transformation matrix from the
	      // interpolated translation and rotation components.
        
	      InterpolationData.at(i).set_size( 4, 4 );
	      InterpolationData.at(i).fill( 0 );
	      for ( int r = 0; r < 3; r++ )
        {
          for ( int c = 0; c < 3; c ++ )
          {
            InterpolationData.at(i).put( r, c, InterpolatedRotationComponent.get(r,c) );
          }
        }
	      InterpolationData.at(i).set_column( 3, InterpolatedTranslationComponent );
	      
	    }
    }
    return InterpolationData;
    
  }
  catch(...)
  {
    std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
		<< ">>>>>>>> In "  << "::Failed to interpolate the given data!!!  Throw up ...\n";
    throw;
  }
  
}   
*/

void Import2DUS::initMaskArrays(int *begin, int *end)
{
  std::cout << "Import2DUS::initMaskArrays() called" << std::endl;
	unsigned char *mask = NULL;
	bool fileMissing = false;
  
  std::stringstream maskFileName;
  maskFileName << this->mFileName << ".msk";
  
  std::ifstream maskFileStream;
  maskFileStream.open(maskFileName.str().c_str());
  if (!maskFileStream.fail())
  {
    
    mask = (unsigned char *)malloc(usData->beams*usData->samples*sizeof(unsigned char));
    for(unsigned int i=0;i<(usData->beams*usData->samples);i++)
      maskFileStream >> mask[i];
    maskFileStream.close();
  }
  else
  {
    std::cout << "Can't read mask file: " << maskFileName.str() 
    << std::endl;
    
    fileMissing = true;
  }
    
	//Initialize begin and end
	for (int y=0; y<usData->beams; y++)
	{
		begin[y] = 0;
		end[y] = 0;
	}
	
	//std::cout << "yDimFS: " << yDimFS << " zDimFS:" << zDimFS << std::endl;
  
	unsigned char pixel = 50;
	for (int y=0; y<usData->beams; y++)
		for (unsigned int x=0; x<usData->samples; x++)
		{
			if (!fileMissing)
			{
				if ( (pixel == 0) && (mask[x*usData->beams+y] == 255) )
				{
					begin[y] = x;
          //std::cout << "b#" << y << ": " << x ;
				}
				else if ( (pixel == 255) && (mask[x*usData->beams+y] == 0) )
        {
					end[y] = x;
          //std::cout << " e#" << y << ": " << x << std::endl;
        }
				pixel = mask[x*usData->beams+y];
			} else { //Couldn´t fint a position file
				begin[y] = 0;
				end[y] = usData->samples;
			}
		}
	
  if (mask)
  {
    free(mask);
    mask = NULL;
  }
}

void Import2DUS::readSettings(Input2DUS* usData, std::string fileName)
{
  if (fileName.empty())
  {
    std::cout << "No settings file" << std::endl;
    return;
  }
  
  std::ifstream settingsFileStream;
  settingsFileStream.open(fileName.c_str());
  if (!settingsFileStream.fail())
  {
    std::string inString, temp;
    while (!settingsFileStream.eof())
    {
      settingsFileStream >> inString;
      //std::cout << inString << std::endl;
      if (inString[0] == '#') // Don't read comment lines
        getline(settingsFileStream, temp);
      else if (inString == "Splat")
      {
        settingsFileStream >> temp; // Equal sign
        settingsFileStream >> usData->sigm_us_x >> usData->sigm_us_y;
        settingsFileStream >> usData->sigm_us_z;
        std::cout << "Splat in mm: " << usData->sigm_us_x << " "
        << usData->sigm_us_y << " " << usData->sigm_us_z << std::endl;
      }
      else if (inString == "Scale")
      {
        settingsFileStream >> temp; // Equal sign
        settingsFileStream >> usData->ellipsoidScale;
        std::cout << "Ellipsoid scale factor: " << usData->ellipsoidScale
        << std::endl;
      }
      else if (inString == "Percentage")
      {
        settingsFileStream >> temp; // Equal sign
        settingsFileStream >> usData->randomNumbersPercentage;
        std::cout << "Random numbers percentage: " 
        << usData->randomNumbersPercentage << std::endl;
      }
      else if (inString == "RMSFile")
      {
        settingsFileStream >> temp; // Equal sign
        settingsFileStream >> usData->rmsFile;
        std::cout << "RMS file: " 
        << usData->rmsFile << std::endl;
      }
      else if (inString == "RemoveFile")
      {
        settingsFileStream >> temp; // Equal sign
        settingsFileStream >> usData->removeFile;
        
        std::cout << "Read random numbers to remove from: " 
        << usData->removeFile << std::endl;
      }
      /*else if (inString == "Remove")
       {
       std::string beam = "";
       while (!settingsFileStream.eof() && beam != "/Remove")
       {
       settingsFileStream >> beam;
       if (beam != "/Remove")
       {
       //std::cout << "Beam: " << beam << " Samples: ";
       std::stringstream line;
       getline(settingsFileStream, temp);
       line << temp;
       std::vector<unsigned int> beamData;
       while (!line.eof())
       {
       unsigned int sample = 0;
       line >> sample;
       beamData.push_back(sample);
       //std::cout << sample << " ";
       }
       usData->removeData.push_back(beamData);
       //std::cout << std::endl;
       }//if
       }//while
       }//else*/
      
    }//while
  }//if
  else
  {
    std::cout << "Error opening settings file for reading: " << fileName;
    std::cout << std::endl;
    throw ("Error reading settings file");
  }
  settingsFileStream.close();
}

void Import2DUS::readRandomNumbersFile(Input2DUS* usData)
                                       //, std::string fileName)
{
  if (usData->hasRandomNumbers)
    throw("Random numbers already exists");
  
  std::ifstream removeFileStream;
  removeFileStream.open(usData->removeFile.c_str());
  
  if (!removeFileStream.fail())
  {
    std::string beam = "";
    std::string temp;
    while (!removeFileStream.eof())
    {
      removeFileStream >> beam;
      //std::cout << "Beam: " << beam << " Samples: ";
      std::stringstream randomline;
      getline(removeFileStream, temp);
      randomline << temp;
      std::list<unsigned int> beamData;
      while (!randomline.eof())
      {
        unsigned int sample = 0;
        randomline >> sample;
        beamData.push_back(sample);
        //std::cout << sample << " ";
      }
      usData->removeData.push_back(beamData);
      //std::cout << std::endl;
    }//while
  }//if
  else
  {
    std::cout << "Error opening .rnd file for reading: " << usData->removeFile;
    std::cout << std::endl;
    throw ("Error reading .rnd file");
  }
  removeFileStream.close();
  usData->hasRandomNumbers = true;
  
}