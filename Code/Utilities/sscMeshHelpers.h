#include <vtkPolyData.h>

#include "sscTool.h"

/**
 * MeshHelpers
 *
 * \date Dec 16, 2010
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */

namespace ssc
{
vtkPolyDataPtr polydataFromTransforms(TimedTransformMap transformMap_prMt);
void loadMeshFromToolTransforms(TimedTransformMap transforms_prMt);

}//namespace ssc

