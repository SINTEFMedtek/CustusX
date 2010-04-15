/*
 * sscDefinitions.cpp
 *
 *  Created on: Apr 14, 2010
 *      Author: christiana
 */

#include "sscDefinitions.h"

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(ssc, ORIENTATION_TYPE, otCOUNT)
{
  "Oblique",
  "Orthogonal"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(ssc, ORIENTATION_TYPE, otCOUNT);

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(ssc, PLANE_TYPE, ptCOUNT)
{
  "NoPlane",
  "Sagittal",
  "Coronal",
  "Axial",
  "Any",
  "Dual",
  "Radial"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(ssc, PLANE_TYPE, ptCOUNT)

SNW_DEFINE_ENUM_STRING_CONVERTERS_BEGIN(ssc, FOLLOW_TYPE, ftCOUNT)
{
  "Follow tool",
  "Fixed center"
}
SNW_DEFINE_ENUM_STRING_CONVERTERS_END(ssc, FOLLOW_TYPE, ftCOUNT)

