//===========================================================================
//
// File: cxInput2DUS.cpp
//
// Created: 06.05.2008
//
/// \file
///       Container for 2D ultrasound data with positions.
/// \author Ole Vegard Solberg
//===========================================================================

//#include "cxEchopac.h"
#include "cxInput2DUS.h"

Input2DUS::Input2DUS() :
	recordID(0),
	firstRecordIndex(0),
	numberOfRecords(0),
	recordSize(0),	
	timeSlice(0),
	frequency(0), 
	probeType(0),
	startDepth(0),
	depthIncrement(0),
	startAngle(0),
	angleIncreament(0),
	acqTimeLine(0),
	samples(0),
	beams(0),
	dataSize(0),
	compRej(0),
  dataRecordSize(0),
  numberOfDataRecords(0),
  numberOfSyncRecords(0),
  beamWith(0),
  sampleDepth(0),
  polar(true),
  sigm_us_x(0.5), 
  sigm_us_y(0.25),
  sigm_us_z(0.25),
  ellipsoidScale(2.5),
  hasRandomNumbers(false),
  randomNumbersPercentage(0),
  rmsFile(""),
  removeFile(""),
  isFinalized(false),
  vtkData(vtkImageData::New())
{
  mOffset[0]=0;
  mOffset[1]=0;
  mOffset[2]=0;
}

Input2DUS::~Input2DUS()
{}

