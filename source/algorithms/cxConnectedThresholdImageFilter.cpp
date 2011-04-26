#include <cxConnectedThresholdImageFilter.h>

namespace cx
{

ConnectedThresholdImageFilter::ConnectedThresholdImageFilter() :
    ThreadedTimedAlgorithm<vtkImageDataPtr>("segmenting", 10)
{

}

ConnectedThresholdImageFilter::~ConnectedThresholdImageFilter()
{
}

void ConnectedThresholdImageFilter::postProcessingSlot()
{
}

vtkImageDataPtr ConnectedThresholdImageFilter::calculate()
{
  return vtkImageDataPtr();
}

}
