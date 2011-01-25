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
class IGTLinkImageMessage : public ImageMessage
{
public:
  typedef IGTLinkImageMessage       Self;
  typedef ImageMessage              Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(IGTLinkImageMessage, igtl::ImageMessage)
  igtlNewMacro(IGTLinkImageMessage);

public:
  // Pack() serializes the header and body based on the member variables.
  // PackBody() must be implemented in the child class.
  int Pack()
  {std::cout << "New pack..." << std::endl;};

private:
};
}
#endif /* CXIGTLINKIMAGEMESSAGE_H_ */
