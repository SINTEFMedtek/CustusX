/*
 * cxIGTLinkSonixStatusMessage.h
 *
 *  Created on: Aug 12, 2011
 */

#ifndef CXIGTLINKSONIXSTATUSMESSAGE_H_
#define CXIGTLINKSONIXSTATUSMESSAGE_H_

#include "igtlMessageBase.h"

namespace cx
{
/**
 * \class IGTLinkSonixStatusMessage
 *
 * \brief
 *
 * \date Aug 12, 2011
 * \author Ole Vegard Solberg, SINTEF
 */
class IGTLCommon_EXPORT IGTLinkSonixStatusMessage : public igtl::MessageBase
{
public:
  typedef IGTLinkSonixStatusMessage       Self;
  typedef igtl::MessageBase               Superclass;
  typedef igtl::SmartPointer<Self>        Pointer;
  typedef igtl::SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(IGTLinkSonixStatusMessage, igtl::MessageBase)
  igtlNewMacro(IGTLinkSonixStatusMessage);

  void SetOrigin(double o[3]);
  void SetOrigin(double oi, double oj, double ok);
  void GetOrigin(double o[3]);
  void GetOrigin(double &oi, double &oj, double &ok);

public:
  // Pack() serializes the header and body based on the member variables.
  // PackBody() must be implemented in the child class.
  int Pack();

protected:
  IGTLinkSonixStatusMessage();
  ~IGTLinkSonixStatusMessage();

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  double  mDataOrigin[3];
  bool    mNewStatus;
//  double  mWidth;

  unsigned char* m_StatusMessage; //All variables are inserted into this before sending

private:
};
}

#endif /* CXIGTLINKSONIXSTATUSMESSAGE_H_ */
