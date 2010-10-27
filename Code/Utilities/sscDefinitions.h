#ifndef SSCDEFINITIONS_H_
#define SSCDEFINITIONS_H_

namespace ssc
{

	enum ORIENTATION_TYPE
	{
		otOBLIQUE,   ///< orient planes relative to the tool space 
		otORTHOGONAL, ///< orient planes relative to the image/reference space.
		otCOUNT
	};
	enum PLANE_TYPE
	{
		ptNOPLANE,     ///< a initial plane, if no yet set
		ptSAGITTAL,   ///< a slice seen from the side of the patient
		ptCORONAL,    ///< a slice seen from the front of the patient
		ptAXIAL,      ///< a slice seen from the top of the patient
		ptANYPLANE,   ///< a plane aligned with the tool base plane
		ptSIDEPLANE,  ///< z-rotated 90* relative to anyplane (dual anyplane)
		ptRADIALPLANE, ///< y-rotated 90* relative to anyplane (bird's view)
		ptCOUNT
	};
	enum FOLLOW_TYPE
	{
		ftFOLLOW_TOOL, ///< center follows tool
		ftFIXED_CENTER, ///< center is set.
		ftCOUNT
	};
	/**Determines the medical domain where the application is to be used.
	 * Use this to adapt components to that domain« terminology/habits.
	 */
	enum MEDICAL_DOMAIN
	{
	  mdLABORATORY,
	  mdNEUROLOGY,
	  mdLAPAROSCOPY,
	  mdCARDIOLOGY,
	  mdCOUNT
	};

	enum MESSAGE_LEVEL
	{
	  mlINFO,
	  mlWARNING,
	  mlERROR,
	  mlDEBUG,
	  mlCOUT,
	  mlCERR,
	  mlSUCCESS,
	  mlCOUNT
	};
} //namespace end

//#include "sscDefinitionStrings.h"

#endif /*SSCDEFINITIONS_H_*/
