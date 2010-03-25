/*
 * cxViewWrapper2D.h
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */
#ifndef CXVIEWWRAPPER2D_H_
#define CXVIEWWRAPPER2D_H_

#include <vector>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "sscData.h"
#include "sscDefinitions.h"
#include "cxViewGroup.h"

namespace cx
{

std::string planeToString(ssc::PLANE_TYPE val);


/** Wrapper for a View2D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper2D : public ViewWrapper
{
  Q_OBJECT
public:
  ViewWrapper2D(ssc::View* view);
  virtual void initializePlane(ssc::PLANE_TYPE plane);
  virtual void setImage(ssc::ImagePtr image);
  virtual void removeImage(ssc::ImagePtr image);
  virtual ssc::View* getView();
private slots:
  void dominantToolChangedSlot(); ///< makes sure the inriareps are connected to the right tool
  void viewportChanged();
  void fixStuff();

private:
  ssc::Vector3D displayToWorld(ssc::Vector3D p_d);
  ssc::Vector3D viewToDisplay(ssc::Vector3D p_v);

  ssc::PLANE_TYPE mPlaneType;
  ssc::SliceProxyPtr mSliceProxy;
  ssc::SliceRepSWPtr mSliceRep;
  //ssc::ToolPtr mTool;
  ssc::ToolRep2DPtr mToolRep2D;
  ssc::OrientationAnnotationRepPtr mOrientationAnnotationRep;
  ssc::DisplayTextRepPtr mPlaneTypeText;
  ssc::View* mView;
};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;


} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
