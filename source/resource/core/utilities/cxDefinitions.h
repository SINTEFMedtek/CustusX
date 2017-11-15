/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXDEFINITIONS_H_
#define CXDEFINITIONS_H_

#include "cxResourceExport.h"

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
 * @{
 */

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

/**
 * @}
 */

} //namespace end

//#include "cxDefinitionStrings.h"

#endif /*CXDEFINITIONS_H_*/
