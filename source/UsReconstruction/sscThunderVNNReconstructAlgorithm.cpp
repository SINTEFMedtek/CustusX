/*
 *  sscThunderVNNReconstructAlgorithm.cpp
 *  Created by Ole Vegard Solberg on 5/6/10.
 */


#include "sscThunderVNNReconstructAlgorithm.h"

#include <QFileInfo>
#include <vtkImageData.h>
#include "recConfig.h"
#ifdef USE_US_RECONSTRUCTION_THUNDER
#include "reconstruct_vnn.h"
#endif // USE_US_RECONSTRUCTION_THUNDER
#include "sscImage.h"
//#include "sscXmlOptionItem.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace ssc 
{
ThunderVNNReconstructAlgorithm::ThunderVNNReconstructAlgorithm(QString shaderPath)
{
  mShaderPath = shaderPath;
}

QString ThunderVNNReconstructAlgorithm::getName() const
{
  return "ThunderVNN";
}

std::vector<DataAdapterPtr> ThunderVNNReconstructAlgorithm::getSettings(QDomElement root)
{
  std::vector<DataAdapterPtr> retval;

#ifdef USE_US_RECONSTRUCTION_THUNDER
	mProcessorOption = StringDataAdapterXml::initialize("Processor", "",
		      "Which processor to use when reconstructing",
		      "CPU", QString("CPU GPU").split(" "),
		      root);
	mDistanceOption = DoubleDataAdapterXml::initialize("Distance", "",
		      "Max distance from frame to voxel when filling output volume. mm.",
		      1, ssc::DoubleRange(0.1, 10, 0.01), 0,
		      root);

	retval.push_back(mProcessorOption);
	retval.push_back(mDistanceOption);
#endif

	return retval;
}
  
void ThunderVNNReconstructAlgorithm::reconstruct(std::vector<TimedPosition> frameInfo, 
                                                 ImagePtr frameData,
                                                 ImagePtr outputData,
                                                 ImagePtr frameMask,
                                                 QDomElement settings)
{
#ifdef USE_US_RECONSTRUCTION_THUNDER
  std::cout << "processor: " << mProcessorOption->getValue() << std::endl;
  std::cout << "distance: " << mDistanceOption->getValue() << std::endl;

  QStringList paths;
  paths << mShaderPath << THUNDER_KERNEL_PATH << ".";

  QFileInfo path;
  path = QFileInfo(paths[0]+"/kernels.ocl");
 if (!path.exists())
   path = QFileInfo(paths[1]+QString("/kernels.ocl"));
  if (!path.exists())
    path = QFileInfo(paths[2]+"/kernels.ocl");
  if (!path.exists())
  {
    ssc::messageManager()->sendError("Error: Can't find kernels.ocl in any of\n  "
                                      + paths.join("  \n"));
    return;
  }
  
  reconstruct_data data;
  vtkImageDataPtr input = frameData->getBaseVtkImageData();
  data.input = static_cast<unsigned char*>(input->GetScalarPointer());
  input->GetDimensions(data.input_dim);
  input->GetSpacing(data.input_spacing);
  
  //test
  long size = data.input_dim[0]*data.input_dim[1]*data.input_dim[2];
  ssc::messageManager()->sendDebug("input size: " 
                                   + qstring_cast(size));
  //ssc::messageManager()->sendInfo("input dimensions: " 
  //                                + string_cast(data.input_dim[0]) + " " 
  //                                + string_cast(data.input_dim[1]) + " " 
  //                                + string_cast(data.input_dim[2]));
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
  //ssc::messageManager()->sendInfo("ThunderVNNReconstructAlgorithm::reconstruct ***success***");
#endif // USE_US_RECONSTRUCTION_THUNDER
}
  
  
  
}//namespace
