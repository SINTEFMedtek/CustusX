#ifndef CXIGTLINKIMAGEMESSAGE_H_
#define CXIGTLINKIMAGEMESSAGE_H_

#include "igtlImageMessage.h"

namespace cx
{
/**
 * \class IGTLinkImageMessage
 *
 * \brief
 *
 * \date Jan 25, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class IGTLCommon_EXPORT IGTLinkImageMessage : public igtl::ImageMessage
{
public:
  typedef IGTLinkImageMessage       Self;
  typedef igtl::ImageMessage        Superclass;
  typedef igtl::SmartPointer<Self>        Pointer;
  typedef igtl::SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(IGTLinkImageMessage, igtl::ImageMessage)
  igtlNewMacro(IGTLinkImageMessage);

public:
  // Pack() serializes the header and body based on the member variables.
  // PackBody() must be implemented in the child class.
  int Pack();

protected:
  IGTLinkImageMessage();
  ~IGTLinkImageMessage();

private:
};
}
#endif /* CXIGTLINKIMAGEMESSAGE_H_ */
