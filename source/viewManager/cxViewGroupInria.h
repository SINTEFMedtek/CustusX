#ifndef CXVIEWGROUPINRIA_H_
#define CXVIEWGROUPINRIA_H_

#include "cxViewGroup.h"

namespace cx
{
/**
 * \class cxViewGroupInria.h
 *
 * \brief
 *
 * \date 6. apr. 2010
 * \author: jbake
 */class ViewGroupInria : public ViewGroup
{
  Q_OBJECT
public:
  ViewGroupInria(int startIndex, ssc::View* view1,
      ssc::View* view2, ssc::View* view3);
  virtual ~ViewGroupInria();

  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);

protected:
  int mStartIndex;
  ssc::ImagePtr mImage;
};
}//namespace cx
#endif /* CXVIEWGROUPINRIA_H_ */
