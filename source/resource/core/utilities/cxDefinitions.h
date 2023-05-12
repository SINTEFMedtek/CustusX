/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDEFINITIONS_H_
#define CXDEFINITIONS_H_

#include "cxResourceExport.h"
#include <QString>

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * @{
 */

extern cxResource_EXPORT const char* TRACKING_SYSTEM_IMPLEMENTATION_IGSTK;
extern cxResource_EXPORT const char* TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK;

	/** orientation type
	 */
	enum cxResource_EXPORT ORIENTATION_TYPE
	{
		otOBLIQUE,   ///< orient planes relative to the tool space
		otORTHOGONAL, ///< orient planes relative to the image/reference space.
		otCOUNT
	};
	enum cxResource_EXPORT PLANE_TYPE
	{
		ptNOPLANE,     ///< a initial plane, if no yet set
		ptSAGITTAL,   ///< a slice seen from the side of the patient
		ptCORONAL,    ///< a slice seen from the front of the patient
		ptAXIAL,      ///< a slice seen from the top of the patient
		ptANYPLANE,   ///< a plane aligned with the tool base plane
		ptSIDEPLANE,  ///< z-rotated 90* relative to anyplane (dual anyplane)
		ptRADIALPLANE, ///< y-rotated 90* relative to anyplane (bird's view)
		ptTOOLSIDEPLANE, ///< z-rotated 90* relative to anyplane like side plane, but always kept oriented like the plane defined by the table up vector/gravity and the tool z vector projected on the horizontal reference plane.
		ptINVERSEANYPLANE,   ///< a plane aligned with the tool base plane, inverse of tool direction
		ptCOUNT
	};
	enum cxResource_EXPORT FOLLOW_TYPE
	{
		ftFOLLOW_TOOL, ///< center follows tool
		ftFIXED_CENTER, ///< center is set.
		ftCOUNT
	};
	/** Determines the general view direction: from the top/superior (neuro) or from the feet/inferir (radio)
	 */
	enum cxResource_EXPORT CLINICAL_VIEW
	{
		mdNEUROLOGICAL,
		mdRADIOLOGICAL,
		mdCOUNT
	};

	enum cxResource_EXPORT MESSAGE_LEVEL
	{
		mlINFO,
		mlWARNING,
		mlERROR,
		mlDEBUG,
		mlCOUT,
		mlCERR,
		mlSUCCESS,
		mlVOLATILE,
		mlRAW,
		mlCOUNT
	};

	enum cxResource_EXPORT LOG_SEVERITY
	{
		msERROR=0,
		msWARNING,
		msINFO,
		msDEBUG,
		msCOUNT
	};

	enum cxResource_EXPORT COORDINATE_SYSTEM
	{
		csREF, ///< the data reference space (r) using LPS (left-posterior-superior) coordinates.
		csDATA, ///< a datas space (d)
		csPATIENTREF, ///< the patient/tool reference space (pr)
		csTOOL, ///< a tools rspace (t)
		csSENSOR, ///< a tools sensor space (s)
		csTOOL_OFFSET, ///< the tool space t with a virtual offset added along the z axis. (to)
		csDATA_VOXEL, ///< the data voxel space (dv)
		csCOUNT
	};

	/** http://www.slicer.org/slicerWiki/index.php/Coordinate_systems
	 */
	enum cxResource_EXPORT PATIENT_COORDINATE_SYSTEM
	{
		pcsLPS, ///< Left-Posterior-Superior, used internally by CustusX, also DICOM, ITK
		pcsRAS, ///< Right-Anterior-Superior, used by Slicer3D, ITK-Snap, nifti, MINC
		pcsCOUNT
	};


	enum cxResource_EXPORT TRACKING_SYSTEM
	{
		tsNONE,             ///< Not specified
		tsPOLARIS,          ///< NDIs Polaris tracker
		tsPOLARIS_SPECTRA,  ///< NDIs Polaris Spectra tracker
		tsPOLARIS_VICRA,    ///< NDIs Polaris Vicra tracker
		tsPOLARIS_CLASSIC,  ///< NDIs Polaris Classic tracker
		tsAURORA,           ///< NDIs Aurora tracker
		tsMICRON,           ///< Claron Technologys Micron tracker
//		tsOPENIGTLINK,      ///< Tracking through OpenIGTLink
		tsCOUNT
	};

	/**Denotes the various states
	 * of registration an object is in.
	 */
	enum cxResource_EXPORT REGISTRATION_STATUS
	{
		rsNOT_REGISTRATED,
		rsIMAGE_REGISTRATED,
		rsPATIENT_REGISTRATED
	};

	enum cxResource_EXPORT STEREOTYPE
	{
		stFRAME_SEQUENTIAL, stINTERLACED, stDRESDEN, stRED_BLUE
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

	enum cxResource_EXPORT IMAGE_MODALITY
	{
		imUNKNOWN,
		imCT,
		imMR,
		imUS,
		imPET,
		imSC,
		imCOUNT
	};
	enum cxResource_EXPORT IMAGE_SUBTYPE
	{
		istUNKNOWN,
		istEMPTY,
		istMRT1,
		istMRT2,
		istMRFLAIR,
		istUSBMODE,
		istANGIO,
		istSEGMENTATION,
		istCOUNT
	};

	enum cxResource_EXPORT LUNG_STRUCTURES
	{
		lsUNKNOWN,
		lsLUNG, // The following enums are used for as buttons in StructuresSelectionWidget. lsLUNG is the first button.
		lsTUMORS,
		lsLESIONS,
		lsLYMPH_NODES,
		lsVENA_AZYGOS,
		lsVENA_CAVA,
		lsAORTA,
		lsSUBCLAVIAN_ARTERY,
		//lsLUNG_VESSELS,
		lsHEART,
		lsESOPHAGUS,
		lsSPINE, // Last button. The values below are other kind of structures, not used used in the button list in StructuresSelectionWidget.
		lsLUNG_VESSELS,
		lsAIRWAYS,
		lsCENTERLINES,
		lsPULMONARY_VEINS,
		lsPULMONARY_TRUNK,
		lsMEDIUM_ORGANS,
		lsSMALL_ORGANS,
		lsNODULES,
		lsCOUNT,
		lsFIRST_STRUCTURE_BUTTON = lsLUNG,
		lsLAST_STRUCTURE_BUTTON = lsSPINE
	};

	enum cxResource_EXPORT LUNG_MODELS
	{
		lmUNKNOWN,
		lmMEDIUM_ORGANS_MEDIASTINUM,
		lmPULMSYST_HEART,
		lmSMALL_ORGANS_MEDIASTINUM,
		lmCOUNT
	};

	enum cxResource_EXPORT LUNG_MODEL_TARGETS
	{
		lmtUNKNOWN,
		lmtVENA_CAVA,
		lmtAORTIC_ARCH,
		lmtASCENDING_AORTA,
		lmtDESCENDING_AORTA,
		lmtSPINE,
		lmtHEART,
		lmtPULMONARY_VEINS,
		lmtPULMONARY_TRUNK,
		lmtBRACHIO_CEPHALIC_VEINS,
		lmtSUBCAR_ART,
		lmtAZYGOS,
		lmtESOPHAGUS,
		lmtCOUNT
	};

/**
 * @}
 */


	static QString airwaysFilterGetNameSuffixCenterline()
	{
		return "_centerline";
	}

	static QString airwaysFilterGetNameSuffixAirways()
	{
		return "Airways"; //"_airways"
	}

	static QString airwaysFilterGetNameSuffixTubes()
	{
		return "_tubes";
	}
	
	static QString airwaysFilterGetNameSuffixColored()
	{
		return "_colored";
	}

	static QString airwaysFilterGetNameSuffixLungs()
	{
		return "_lungs";
	}

	static QString airwaysFilterGetNameSuffixLungVessels()
	{
		return "_lung_vessels";
	}

	static QString airwaysFilterGetNameSuffixVolume()
	{
		return "_volume";
	}

} //namespace end

//#include "cxDefinitionStrings.h"

#endif /*CXDEFINITIONS_H_*/
