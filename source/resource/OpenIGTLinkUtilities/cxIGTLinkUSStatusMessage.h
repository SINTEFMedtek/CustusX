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


#ifndef CXIGTLINKUSSTATUSMESSAGE_H_
#define CXIGTLINKUSSTATUSMESSAGE_H_

#include "igtlMessageBase.h"
#include "cxProbeDefinition.h"

namespace cx
{

/**
 * \ingroup cx_resource_OpenIGTLinkUtilities
 * \date Apr 16, 2012
 * \author Ole Vegard Solberg, SINTEF
 * \brief
IGTLink Message content:

int Type: Probe Type =
  1 = sector
  2 = linear

double OriginX
double OriginY : Origin of sector in image coordinates

double DepthStart: Start of sector in mm from origin
double DepthEnd: End of sector in mm from origin
double Width: Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.

String DataFormat: Format of image. B-mode, Angio, Beamdata, etc.

 */
class IGTLCommon_EXPORT IGTLinkUSStatusMessage : public igtl::MessageBase
{
public:
  typedef IGTLinkUSStatusMessage      		Self;
  typedef igtl::MessageBase               Superclass;
  typedef igtl::SmartPointer<Self>        Pointer;
  typedef igtl::SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(IGTLinkUSStatusMessage, igtl::MessageBase)
	igtlNewMacro(IGTLinkUSStatusMessage)

	void SetProbeType(ProbeDefinition::TYPE probeType);
  int GetProbeType();
  void SetOrigin(double o[3]);
  void SetOrigin(double oi, double oj, double ok);
  void GetOrigin(double o[3]);
  void GetOrigin(double &oi, double &oj, double &ok);
  const double* GetOrigin() const;
  void SetDepthStart(double depthStart);
  double GetDepthStart();
  void SetDepthEnd(double depthEnd);
  double GetDepthEnd();
  void SetWidth(double width);
  double GetWidth();
  void SetDataFormat(std::string dataFormat);
  std::string GetDataFormat();

protected:
	IGTLinkUSStatusMessage();
	virtual ~IGTLinkUSStatusMessage();

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
	virtual int  UnpackContent();

	ProbeDefinition::TYPE mProbeType; 				///< 1 = linear, 2 = sector
  double mDataOrigin[3];	///< Origin of sector in image coordinates
  double mDepthStart;			///< Start of sector in mm from origin
  double mDepthEnd;				///< End of sector in mm from origin
  double mWidth;					///< Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.
  std::string mDataFormat;///< Format of image. B-mode, Angio, Beamdata, etc.

  unsigned char* m_StatusMessage; //All variables are inserted into this before sending
};

}

#endif /* CXIGTLINKUSSTATUSMESSAGE_H_ */
