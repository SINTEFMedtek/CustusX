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
class ROI
{
public:
    /// roi - upper left (x)
    int ulx;
    /// roi - upper left (y)
    int uly;
    /// roi - upper right (x)
    int urx;
    /// roi - upper right (y)
    int ury;
    /// roi - bottom right (x)
    int brx;
    /// roi - bottom right (y)
    int bry;
    /// roi - bottom left (x)
    int blx;
    /// roi - bottom left (y)
    int bly;
};

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
  void GetRIO(double o[8]);

public:

protected:
  IGTLinkSonixStatusMessage();
  ~IGTLinkSonixStatusMessage();

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  double mDataOrigin[3];
  ROI mROI; //Ulterius Region of Interest
  int mNewStatus;
//  double  mWidth;

  unsigned char* m_StatusMessage; //All variables are inserted into this before sending

private:
};
}

#endif /* CXIGTLINKSONIXSTATUSMESSAGE_H_ */
