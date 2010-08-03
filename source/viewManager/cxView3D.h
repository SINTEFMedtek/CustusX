#ifndef CXVIEW3D_H_
#define CXVIEW32_H_

#include "sscView.h"
#include "sscTransform3D.h"
#include "sscTool.h"

class QWidget;
class QMenu;

namespace cx
{
//class DataManager;
class RepManager;
class MessageManager;
//class ToolManager;

typedef ssc::Transform3D Transform3D;
/**
 * \class View3D
 *
 * \brief Class for displaying 3D reps.
 *
 * \date Dec 9, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class View3D : public ssc::View
{
  Q_OBJECT
public:
  enum CameraStyle{
    DEFAULT_STYLE,
    TOOL_STYLE
  };

  View3D(const std::string& uid, const std::string& name="", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
  virtual ~View3D(); ///< empty

  virtual Type getType() const { return VIEW_3D;}; ///< get the class type

  void setCameraStyle(CameraStyle style, int offset=-1); ///< lets the user select where the camera, offset only used by TOOL_STYLE

public slots:
  void setCameraOffsetSlot(int offset); ///< sets the camera offset
  
protected slots:
  void moveCameraToolStyleSlot(Transform3D prMt, double timestamp); ///< receives transforms from the tool which the camera should follow

protected:
  void activateCameraDefaultStyle();
  void activateCameraToolStyle(int offset=0);
  void deactivateCameraToolStyle();

  CameraStyle mCameraStyle; ///< the current camerastyle
  int mCameraOffset; ///< the distance between the camera and focalpoint
  ssc::ToolPtr mFollowingTool; ///< the tool the camera is following
};
}//namespace cx
#endif /* CXVIEW3D2_H_ */
