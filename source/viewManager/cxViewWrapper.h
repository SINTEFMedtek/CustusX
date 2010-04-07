#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include <QObject>
#include "sscImage.h"
#include "sscView.h"
#include "sscDefinitions.h"

namespace cx
{
/**
 * \class cxViewWrapper.h
 *
 * \brief
 *
 * \date 6. apr. 2010
 * \author: jbake
 */
class ViewWrapper : public QObject
{
  Q_OBJECT
public:
  virtual ~ViewWrapper() {}
  virtual void initializePlane(ssc::PLANE_TYPE plane) {}
  virtual void setImage(ssc::ImagePtr image) = 0;
  virtual void removeImage(ssc::ImagePtr image) = 0;
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode) {}
  virtual ssc::View* getView() = 0;
  virtual void setZoom(double zoomFactor, const ssc::Vector3D& click_vp) {}
};
typedef boost::shared_ptr<ViewWrapper> ViewWrapperPtr;

}//namespace cx
#endif /* CXVIEWWRAPPER_H_ */
