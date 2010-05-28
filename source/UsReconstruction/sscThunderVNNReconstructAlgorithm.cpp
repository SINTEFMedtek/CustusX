/*
 *  sscThunderVNNReconstructAlgorithm.cpp
 *  Created by Ole Vegard Solberg on 5/6/10.
 */


#include "sscThunderVNNReconstructAlgorithm.h"

#include <QFileInfo>
#include <vtkImageData.h>
#include "sscConfig.h"
#include "reconstruct_vnn.h"
#include "sscImage.h"
#include "sscXmlOptionItem.h"

namespace ssc 
{
ThunderVNNReconstructAlgorithm::ThunderVNNReconstructAlgorithm()
{
}

void ThunderVNNReconstructAlgorithm::getSettings(QDomElement root)
{
  StringOptionItem::initialize("Processor",
      "",
      "Which processor to use when reconstructing",
      "CPU",
      "\"CPU\" \"GPU\"",
      root);
}
  
void ThunderVNNReconstructAlgorithm::reconstruct(std::vector<TimedPosition> frameInfo, 
                                                 ImagePtr frameData,
                                                 ImagePtr outputData,
                                                 ImagePtr frameMask)
{
  QFileInfo path(SSC_THUNDER_KERNEL_PATH+QString("/kernels.ocl"));
  
  if (!path.exists())
    path = QFileInfo("./kernels.ocl");
  if (!path.exists())
  {
    std::cout << "Error: Can't find kernels.ocl" << std::endl;
    return;
  }
  
  reconstruct_data data;
  vtkImageDataPtr input = frameData->getBaseVtkImageData();
  data.input = static_cast<unsigned char*>(input->GetScalarPointer());
  input->GetDimensions(data.input_dim);
  input->GetSpacing(data.input_spacing);
  
  //test
  long size = data.input_dim[0]*data.input_dim[1]*data.input_dim[2];
  std::cout << "input size: " << size << std::endl;
  std::cout << "input dimensions: " << data.input_dim[0] << " " << data.input_dim[1] << " " << data.input_dim[2] << std::endl;
  //for (int i = 0; i < size; i++)
  //  data.input[i]=255;
  
  data.input_pos_matrices = new double[frameInfo.size()*12];
  for (unsigned int i = 0; i < frameInfo.size(); i++)
  {
    boost::array<double, 16> m =  frameInfo[i].mPos.flatten(); 
    for (int j = 0; j < 12; j++)
      data.input_pos_matrices[12*i + j] = m[j];
  }
  
  vtkImageDataPtr input_mask = frameMask->getBaseVtkImageData();
  data.input_mask = static_cast<unsigned char*>(input_mask->GetScalarPointer());
  
  vtkImageDataPtr output = outputData->getBaseVtkImageData();
  data.output = static_cast<unsigned char*>(output->GetScalarPointer());
  output->GetDimensions(data.output_dim);
  output->GetSpacing(data.output_spacing);
  
  reconstruct_vnn(&data, path.absoluteFilePath().toStdString().c_str());
  std::cout << "ThunderVNNReconstructAlgorithm::reconstruct ***success***" << std::endl;
}
  
  
  
}//namespace
