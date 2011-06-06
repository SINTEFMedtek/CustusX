//===========================================================================
//
// File: cxInput2DUS.h
//
// Created: 06.05.2008
//
/// \file
///       Container for 2D ultrasound data with positions.
/// \author Ole Vegard Solberg
//===========================================================================

#ifndef _INPUT2DUS_H_
#define _INPUT2DUS_H_

#include <vector>
#include <vtkImageData.h>
#include <itkAffineTransform.h>

#include "itkImageFileReader.h"

typedef itk::AffineTransform<double, 3>	AffineTransformType;
typedef std::vector<AffineTransformType::ParametersType> PositionVectorType;

class Input2DUS
{
public: 
  
	unsigned short recordID;
	unsigned long firstRecordIndex;
	unsigned long numberOfRecords;
	unsigned long recordSize;
	
	float timeSlice; // Time slice in seconds. Always 0.004 Trace and sync records use this.
	float frequency; // Frequency tissue probe

	signed short probeType;

	// Status tissue
	float startDepth;
	float depthIncrement;
	float startAngle;
	float angleIncreament;
	float acqTimeLine;
	//signed short samples;
	unsigned int samples;
	signed short beams;
	signed short dataSize;
	signed short compRej;

  unsigned long dataRecordSize;
	unsigned long numberOfDataRecords;
	unsigned long numberOfSyncRecords;
  
  double beamWith;
  double sampleDepth;

  bool polar;//Polar coordinates
  
  double sigm_us_x, sigm_us_y, sigm_us_z, ellipsoidScale;
  
  // Comparison test: Remove random data from middle slice
  bool hasRandomNumbers;
  unsigned int randomNumbersPercentage;
  std::string rmsFile;
  std::string removeFile;
  //std::vector<std::vector<unsigned int> > removeData;
  std::vector<std::list<unsigned int> > removeData;
  
  std::vector<std::vector<float> > *positions;
  std::vector<std::vector<float> > *finalPositions;
  bool isFinalized;
  
  std::vector<signed long> *syncVector;

	//std::vector<frame*> frames;
	
	//template <typename sample>
	
	//typedef std::vector<char> beam;
	//typedef std::vector<beam> frame;
	//std::vector<frame> volum;

  
  
  vtkImageData* vtkData;
  AffineTransformType::Pointer calibration;
  AffineTransformType::Pointer targetVolumeTransform;
  PositionVectorType positionVector;
  
  //std::vector<int> mOffset;
  float mOffset[3];
  
  std::vector<double> mTimestamps;
  std::vector<double> mPosTimestamps;
  
  int *sectorBeginArray;
  int *sectorEndArray;
	
  Input2DUS();
	virtual ~Input2DUS();
  //virtual void test();
  
  void importmhd(const char *filemane);

private:

};

#endif