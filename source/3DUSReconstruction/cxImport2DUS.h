//===========================================================================
//
// File: cxImport2DUS.h
//
// Created: 09.05.2008
//
//
/// \file
///       Container for 2D ultrasound data with positions.
/// \author Ole Vegard Solberg
//===========================================================================

#ifndef _IMPORT2DUS_H_
#define _IMPORT2DUS_H_

#include <vector>
//#include <itkAffineTransform.h>

#include "cxInput2DUS.h"
//#include "itkImageFileReader.h"

class Import2DUS
{
public:
  
  //typedef itk::AffineTransform<double, 3>	AffineTransformType;  
	//typedef std::vector<AffineTransformType::ParametersType> PositionVectorType;
  
	Import2DUS();
	virtual ~Import2DUS();
  
  /**
   * Read a meta header file
   * \param filename Filename of the meta header file
   * \param oldDataFormat Read data on old format without time tags
   */
  void importmhd(const char *filename, 
                 bool oldDataFormat = false,
                 double timeCalibration = 0.0);
  void readInputFile(const char *filename);
  /**
   * Read position file and position timetags
   * \param fileName The start of the file name for positions and position 
   * \param oldDataFormat Read data on old format without time tags
   * time tags. If fileName is supplied, the position file 
   * is assumed to be fileName + "_transforms.txt" and the position time tag 
   * file is assumed to be fileName + "_timestamps.txt". Position time tags 
   * are only read if fileName is supplied.
   * If left empty a file name of mFileName + ".pos" is assumed. 
   */
  void readPosStackFile(std::string fileName = "", 
                        bool oldDataFormat = false,
                        double timeCalibration = 0.0);
  
  /**
   * Read timestamp file
   * \param timeVector The vector to store the timestamps in
   * \param fileName File name of time tag file. If left empty a file name of
   * mFileName + "_timestamps.txt" is assumed.
   */
  void readTimeTags(std::vector<double> *timeVector, std::string fileName = "");
  /**
   * Syncronize positions and images
   */
  void syncronize(int numPositions, PositionVectorType& rfMtd_ParamsAll);
  /**
   * Syncronise positions and images when we have both position and image time
   * tags. 
   * Works by placing the image time tag between two position time tags and 
   * picks the closest.
   * \param oldPositionVector Input positions
   * \param timeCal This value will be added to the image timetags.
   * If omitted or set to 0.0 a time calibration will be made by matching 
   * the first image timetag with the first position timetag.
   */
  void syncronize(PositionVectorType& oldPositionVector, double timeCal = 0.0);
  
  /**
   * Read calibration file into a transform variable. 
   * May also read other transforms 
   * (like a rotation of the target volume system).
   * \param calibration The variable to read the calibration into
   * \param fileName The file name of the transform
   */
  void readCalFile(/*AffineTransformType::Pointer calibration,
                   std::string fileName*/);
  
  vtkImageData* getOutput();
  Input2DUS* getUsData();
  
/*  std::vector<vnl_matrix<double> > matrixInterpolation(
    vnl_vector<double> DataPoints,
    std::vector<vnl_matrix<double> > DataValues,
    vnl_vector<double> InterpolationPoints,
    std::string InterpolationMethod );*/
  
  void initMaskArrays(int *begin, int *end);
  
  void readSettings(Input2DUS* usData, std::string fileName);
  void readRandomNumbersFile(Input2DUS* usData);
  
private:
  Input2DUS *usData;
  std::string mFileName;
};

#endif

//===========================================================================
// $Log:$
//
//===========================================================================