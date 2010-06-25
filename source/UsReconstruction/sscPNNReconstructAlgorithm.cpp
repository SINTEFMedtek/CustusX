/*
 *  sscPNNReconstructAlgorithm.cpp
 *  Created by Ole Vegard Solberg on 23/6/10.
 */


#include "sscPNNReconstructAlgorithm.h"

#include <QFileInfo>
#include "recConfig.h"
#include "sscXmlOptionItem.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "utils/sscReconstructHelper.h"
//#include "sscReconstructer.h"

namespace ssc 
{
PNNReconstructAlgorithm::PNNReconstructAlgorithm()
{
}

void PNNReconstructAlgorithm::getSettings(QDomElement root)
{

}

  
void optimizedCoordTransform(ssc::Vector3D* p, boost::array<double, 16> tt)
{
  double* t = tt.begin();
  double x = (*p)[0];
  double y = (*p)[1];
  double z = (*p)[2];
  (*p)[0] = t[0]*x + t[1]*y + t[2]*z + t[3];
  (*p)[1] = t[4]*x + t[5]*y + t[6]*z + t[7];
  (*p)[2] = t[8]*x + t[9]*y + t[10]*z + t[11];
}
  
void PNNReconstructAlgorithm::reconstruct(std::vector<TimedPosition> frameInfo, 
                                                 ImagePtr frameData,
                                                 ImagePtr outputData,
                                                 ImagePtr frameMask,
                                                 QDomElement settings)
{
  //std::vector<Planes> planes = generate_planes(frameInfo, frameData);
  
  vtkImageDataPtr input = frameData->getBaseVtkImageData();
  vtkImageDataPtr target = outputData->getBaseVtkImageData();
  
  int* inputDims = input->GetDimensions();
  //ssc::Vector3D inputDims(input->GetDimensions());
  
  ssc::Vector3D targetDims(target->GetDimensions());
  ssc::Vector3D targetSpacing(target->GetSpacing());
  
  //Create temporary volume
  vtkImageDataPtr tempOutput = generateVtkImageData(targetDims, targetSpacing, 0);
  ImagePtr tempOutputData = ImagePtr(new Image("tempOutput", tempOutput, "tempOutput")) ;
  
  int* outputDims = tempOutput->GetDimensions();
  //int* outputDims = target->GetDimensions();
  
  if(inputDims[2] != static_cast<int>(frameInfo.size()))
    messageManager()->sendWarning("inputDims[2] != frameInfo.size()" 
                                  + string_cast(inputDims[2]) + " != " 
                                  + string_cast(frameInfo.size()));
  
  ssc::Vector3D inputSpacing(input->GetSpacing());
  ssc::Vector3D outputSpacing(tempOutput->GetSpacing());
  
  //Get raw data pointers
  unsigned char *inputPointer = static_cast<unsigned char*>( input->GetScalarPointer() );
  unsigned char *outputPointer = static_cast<unsigned char*>(tempOutput->GetScalarPointer());
  //unsigned char *outputPointer = static_cast<unsigned char*>(target->GetScalarPointer());
  unsigned char* maskPointer = static_cast<unsigned char*>(frameMask->getBaseVtkImageData()->GetScalarPointer());
  
  // Traverse all input pixels
  for (int record = 0; record < inputDims[2]; record++)
  {
    //messageManager()->sendDebug("record: " + string_cast(record));
    boost::array<double, 16> recordTransform = frameInfo[record].mPos.flatten();
    
    for (int beam = 0; beam < inputDims[0]; beam++)
    {
      for (int sample = 0; sample < inputDims[1]; sample++)
      {
        if(!validPixel(beam, sample, inputDims, maskPointer))
          continue;
        ssc::Vector3D inputPoint(beam*inputSpacing[0], sample*inputSpacing[1], 0.0);
        //ssc::Vector3D outputPoint = frameInfo[record].mPos.coord(inputPoint);
        ssc::Vector3D outputPoint = inputPoint;
        optimizedCoordTransform(&outputPoint, recordTransform);
        //ssc::Vector3D outputVoxel;
        int outputVoxelX = static_cast<int>((outputPoint[0] / outputSpacing[0]) + 0.5);
        int outputVoxelY = static_cast<int>((outputPoint[1] / outputSpacing[1]) + 0.5);
        int outputVoxelZ = static_cast<int>((outputPoint[2] / outputSpacing[2]) + 0.5);
        
        if (validVoxel(outputVoxelX, outputVoxelY, outputVoxelZ, outputDims))
        {
          int outputIndex = outputVoxelX 
                          + outputVoxelY*outputDims[0] 
                          + outputVoxelZ*outputDims[0]*outputDims[1];
          int inputIndex = beam 
                         + sample*inputDims[0]
                         + record*inputDims[0]*inputDims[1];
          outputPointer[outputIndex] = inputPointer[inputIndex];
        }//validVoxel
        
      }//sample
    }//beam
  }//record
  
  // Fill holes
  this->interpolate(tempOutputData, outputData);
}
    
  
void PNNReconstructAlgorithm::interpolate(ImagePtr inputData,
                                          ImagePtr outputData)
{
  messageManager()->sendInfo("Interpolating...");
  
  
  vtkImageDataPtr input = inputData->getBaseVtkImageData();
  vtkImageDataPtr output = outputData->getBaseVtkImageData();
  
  //int* inputDims = input->GetDimensions();
  int* outputDims = output->GetDimensions();
  
  ssc::Vector3D inputDims(input->GetDimensions());
  //ssc::Vector3D outputDims(output->GetDimensions());
  
  
  unsigned char *inputPointer = static_cast<unsigned char*>(input->GetScalarPointer());
  unsigned char *outputPointer = static_cast<unsigned char*>(output->GetScalarPointer());
  
  if((outputDims[0] != inputDims[0])
     || (outputDims[1] != inputDims[1])
     || (outputDims[2] != inputDims[2]))
    messageManager()->sendWarning("outputDims != inputDims. output: " +
                                  string_cast(outputDims[0]) + " " +
                                  string_cast(outputDims[1]) + " " +
                                  string_cast(outputDims[2]) + " input: " +
                                  string_cast(inputDims[0]) + " " +
                                  string_cast(inputDims[1]) + " " +
                                  string_cast(inputDims[2]));
  
  // Traverse all voxels
  for (int x = 0; x < outputDims[0]; x++)
  {
    //messageManager()->sendDebug("x: " + string_cast(x));
    for (int y = 0; y < outputDims[1]; y++)
      for (int z = 0; z < outputDims[2]; z++)
      {
        int outputIndex = x 
        + y * outputDims[0] 
        + z * outputDims[0]*outputDims[1];
        bool interpolated = false;
        int localArea = 0;
        int interpolationSteps = 3;//TODO: set as input variable
        
        int count = 0;
        double tempVal = 0;
        do
        {
          for (int i = -localArea; i < localArea+1; i++)
            for (int j = -localArea; j < localArea+1; j++)
              for (int k = -localArea; k < localArea+1; k++)
              {
                //optimize? - The if is to expensive
                /*if((i == -localArea || i == localArea ||
                   j == -localArea || j == localArea ||
                   k == -localArea || k == localArea))*/
                  //continue;
                /*if(i != -localArea || i != localArea ||
                 j != -localArea || j != localArea ||
                 k != -localArea || k != localArea)*/
                {
                
                int localIndex = outputIndex 
                + i 
                + j*outputDims[0] 
                + k*outputDims[0]*outputDims[1];
                
                if (validVoxel(x+i, y+j, z+k, outputDims)
                    && inputPointer[localIndex] > 0.1)
                {
                  tempVal += inputPointer[localIndex];
                  count++;
                }
                }// if optimize
              }//local voxel area
          if (count > 0)
          {
            interpolated = true;
            outputPointer[outputIndex] = static_cast<int>((tempVal/count) + 0.5);
          }
          localArea++;
        } while (localArea <= interpolationSteps && !interpolated);
        
      }//all voxels
  }//x
}
  
}//namespace
