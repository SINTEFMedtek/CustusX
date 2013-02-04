#ifndef SSCDEFINITIONS_H_
#define SSCDEFINITIONS_H_

namespace ssc
{

/**
 * \addtogroup sscUtility
 * @{
 */

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
	/**Determines the clinical application where the system is to be used.
	 * Use this to adapt components to that clinical application� terminology/habits.
	 */
	enum CLINICAL_APPLICATION
	{
		mdLABORATORY,
		mdNEUROLOGY,
		mdLAPAROSCOPY,
		mdENDOVASCULAR,
		mdBRONCHOSCOPY,
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

	enum COORDINATE_SYSTEM
	{
		csREF, ///< the data reference space (r)
		csDATA, ///< a datas space (d)
		csPATIENTREF, ///< the patient/tool reference space (pr)
		csTOOL, ///< a tools rspace (t)
		csSENSOR, ///< a tools sensor space (s)
		csTOOL_OFFSET, ///< the tool space t with a virtual offset added along the z axis.
		csDATA_VOXEL, ///< the data voxel space
		csCOUNT
	};

	enum TRACKING_SYSTEM
	{
		tsNONE,             ///< Not specified
		tsPOLARIS,          ///< NDIs Polaris tracker
		tsPOLARIS_SPECTRA,  ///< NDIs Polaris Spectra tracker
		tsPOLARIS_VICRA,    ///< NDIs Polaris Vicra tracker
		tsPOLARIS_CLASSIC,  ///< NDIs Polaris Classic tracker
		tsAURORA,           ///< NDIs Aurora tracker
		tsMICRON,           ///< Claron Technologys Micron tracker
		tsCOUNT
	};

	/*enum TOOL_TYPE
	{
		ttNONE,
		ttREFERENCE,  ///< references
		ttMANUAL,     ///< representation of a mouse/keyboard-controlled virtual tool
		ttPOINTER,    ///< pointers
		ttUSPROBE,    ///< ultrasound probe
		ttCOUNT
	};*/

/**
 * @}
 */

} //namespace end

//#include "sscDefinitionStrings.h"

#endif /*SSCDEFINITIONS_H_*/
