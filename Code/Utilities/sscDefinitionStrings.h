#ifndef SSCDEFINITIONSTRINGS_H_
#define SSCDEFINITIONSTRINGS_H_

#include "sscDefinitions.h"
#include "sscEnumConverter.h"

// put compiler-intensive templates here instead of in the much used sscDefinitions.h

namespace ssc
{
} //namespace end

SNW_DECLARE_ENUM_STRING_CONVERTERS(ssc, ORIENTATION_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(ssc, PLANE_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(ssc, FOLLOW_TYPE);
SNW_DECLARE_ENUM_STRING_CONVERTERS(ssc, MEDICAL_DOMAIN);
SNW_DECLARE_ENUM_STRING_CONVERTERS(ssc, MESSAGE_LEVEL);

#endif /*SSCDEFINITIONSTRINGS_H_*/
