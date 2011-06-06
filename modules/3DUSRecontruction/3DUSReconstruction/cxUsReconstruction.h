//===========================================================================
//
// File: usreconstruction.h
//
// Created: 12.0.2007
//
// Revision: $Id:$
//
/// \file
///       Ultrasound reconstruction algorithms
/// \author Ole Vegard Solberg
//===========================================================================

#ifndef _USRECONSTRUCTION_H_
#define _USRECONSTRUCTION_H_

#include "cxInput2DUS.h"
#include "cxInput2DUStemplate.h"
#include "cxInput2DUS.h"

#include <itkAffineTransform.h>
#include <itkImage.h>
#include <vtkImageData.h>
//#include <itkImageFileReader.h>
#include <itkRescaleIntensityImageFilter.h>
#include "itkVtkGlue/itkImageToVTKImageFilter.h"

// Move to a volume class
#include <vtkImageWriter.h>

typedef itk::AffineTransform<double, 3>	TransformType; 
typedef std::vector<TransformType::ParametersType> PositionVectorType;
typedef itk::Image< double, 3 > TempVolumeType;
typedef itk::Image<unsigned short, 3> FinalVolumeType;

class UsReconstruction
{
private:
  PositionVectorType inputPositions;
  // Transform from input image positions to output volume position
  TransformType::Pointer m_targetVolumeTransform;
  TransformType::Pointer m_calibration;
  TransformType::Pointer m_moveOrigo;
  
  //Volume size
  //double m_rcLengthX, m_rcLengthY, m_rcLengthZ;
  TempVolumeType::SizeType m_volumeSize;
  TempVolumeType::InternalPixelType m_volumeSpacing[TempVolumeType::ImageDimension];
    
  //void importPositions(Input2DUSTemplate<char>* et,
  //                     std::vector<std::vector<float> > *positions);
  /**
   * Find target volume orientation and rotation
   */
  void findVolumeOrientationAndRotation(Input2DUS* usData, 
                                        bool alignMiddle = true,
                                        std::string 
                                        mTargetVolumeTransformFile = "");
  
  /**
   * Set calibration matrix
   */
  void setCalibration(TransformType::Pointer cal);
  
  /**
   * Calculate target volume size
   */
  void findVolumeSize(Input2DUS* usData, std::string outputFileName);
  
  /**
   * Allocate memory for volume buffer
   */
  void allocateVolume(TempVolumeType::Pointer& volume);
  //void findInputPoint(Input2DUSTemplate<char>* et, 
  //                    TransformType::InputPointType& point, bool polar);
  
  double min(double x, double y) { 
    return x<y?x:y; }
  double max(double x, double y) { 
    return x>y?x:y; }
  double min(double a, double b, double c, double d, double e) { 
    return min(min(min(a,b),min(c,d)),e); } 
  double max(double a, double b, double c, double d, double e) {
    return max(max(max(a,b),max(c,d)),e); }
  void readParams(TransformType::ParametersType& params,
                                    std::vector<float> pos) { 
    params[0] = pos[0];    params[1] = pos[1];    params[2] = pos[2];
    params[3] = pos[4];    params[4] = pos[5];    params[5] = pos[6];
    params[6] = pos[8];    params[7] = pos[9];    params[8] = pos[10];
    params[9] = pos[3];    params[10] = pos[7];    params[11] = pos[11]; }
  
  void readParams(TransformType::ParametersType& params,
                  AffineTransformType::ParametersType pos) { 
    params[0] = pos[0];    params[1] = pos[1];    params[2] = pos[2];
    params[3] = pos[3];    params[4] = pos[4];    params[5] = pos[5];
    params[6] = pos[6];    params[7] = pos[7];    params[8] = pos[8];
    params[9] = pos[9];    params[10] = pos[10];    params[11] = pos[11]; }
  
  
public:
    enum ReconstructionTypeEnum { 
      PNN, 
      kernel = 10 }; //3D kernel, Often called DW
      //DW = 10};
    
  UsReconstruction();
  ~UsReconstruction();
  /**
   * Reconstruct ultrasound 3D volume
   */
  vtkImageData* reconstructVolume(Input2DUS* usData, 
                                  std::string outputFileName,
                                  ReconstructionTypeEnum type = PNN,
                                  double spacing = 0.5,
                                  int interpolationSteps = 2,
                                  bool alignMiddle = true,
                                  std::string mTargetVolumeTransformFile = "",
                                  //std::string settingsFile = "",
                                  bool maxRange = false,
                                  unsigned int removeFromRecord = 0);
  
  vtkImageData* interpolate(vtkImageData* inputVolume,
                            int interpolationSteps = 2);
  /**
   * Write .mha anf .raw file
   */
  void writeFile(vtkImageData* volume, std::string filename);
  
  void writeTransform(AffineTransformType::Pointer transform,
                      TempVolumeType::SizeType volumeSize,
                      std::string fileName);
  
  void readTransform(AffineTransformType::Pointer transform,
                     //TempVolumeType::SizeType volumeSize,
                     std::string fileName);
    
  void findRandomNumbers(Input2DUS* usData,
                         std::string fileName);
  
  unsigned int random(unsigned int max);
};
  
#endif
