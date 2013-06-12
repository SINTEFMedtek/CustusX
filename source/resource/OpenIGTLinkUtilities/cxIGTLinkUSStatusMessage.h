// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.


#ifndef CXIGTLINKUSSTATUSMESSAGE_H_
#define CXIGTLINKUSSTATUSMESSAGE_H_

#include "igtlMessageBase.h"

namespace cx
{

/**
 * \class IGTLinkUSStatusMessage
 * \ingroup cxResourceOpenIGTLinkUtilities
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
  igtlNewMacro(IGTLinkUSStatusMessage);

  void SetProbeType(int probeType);
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
  virtual int  UnpackBody();

  int mProbeType; 				///< 1 = linear, 2 = sector
  double mDataOrigin[3];	///< Origin of sector in image coordinates
  double mDepthStart;			///< Start of sector in mm from origin
  double mDepthEnd;				///< End of sector in mm from origin
  double mWidth;					///< Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.
  std::string mDataFormat;///< Format of image. B-mode, Angio, Beamdata, etc.

  unsigned char* m_StatusMessage; //All variables are inserted into this before sending
};

}

#endif /* CXIGTLINKUSSTATUSMESSAGE_H_ */
