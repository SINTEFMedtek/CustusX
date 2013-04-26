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

/**
 * \addtogroup sscUtility
 * \{
 */

vtkPolyDataPtr polydataFromTransforms(TimedTransformMap transformMap_prMt);
void loadMeshFromToolTransforms(ssc::TimedTransformMap transforms_prMt);

/**
 * Get information about a ssc mesh.
 */
std::map<std::string, std::string> getDisplayFriendlyInfo(ssc::MeshPtr mesh);

/**
 * \}
 */

}//namespace ssc

