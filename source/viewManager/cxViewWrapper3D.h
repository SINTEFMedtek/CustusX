/*
 * cxViewWrapper3D.h
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */
#ifndef CXVIEWWRAPPER3D_H_
#define CXVIEWWRAPPER3D_H_

#include <vector>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"

namespace cx
{


/** Wrapper for a View3D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper3D : public ViewWrapper
{
  Q_OBJECT
public:
  ViewWrapper3D(int startIndex, ssc::View* view);
  //ssc::View* initialize(ssc::PLANE_TYPE plane);
  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual void setRegistrationMode(ssc::REGISTRATION_STATUS mode);
  virtual ssc::View* getView();

private slots:
  void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
  void toolManagerConfiguredSlot(); ///< add all tools when configured

private:
  ssc::VolumetricRepPtr mVolumetricRep;
  LandmarkRepPtr mLandmarkRep;
  ssc::ProbeRepPtr mProbeRep;
  ssc::ImagePtr mImage;
  ssc::View* mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
