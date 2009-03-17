#ifndef CXVIEW3D_H_
#define CXVIEW32_H_

#include "sscView.h"

class QWidget;
class QMenu;

namespace ssc
{
class DataManager;
}
namespace cx
{
class RepManager;
class MessageManager;

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
typedef ssc::DataManager DataManager;

public:
  View3D(const std::string& uid, const std::string& name="", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
  virtual ~View3D(); ///< empty

  virtual Type getType() const { return VIEW_3D;}; ///< get the class type
  void contextMenuEvent(QContextMenuEvent *event); ///< decides what happens when you rightclick in a view

protected:
  QMenu* mContextMenu; ///< right click menu
  QMenu* mMakeVolumetricRepMenu; ///< sub menu of the context menu, for choosing volumetric reps
  QMenu* mMakeGeometricRepMenu; ///< sub menu of the context menu, for choosing geometric reps

  DataManager* mDataManager; ///< has all the data loaded into the system
  RepManager* mRepManager; ///< has a pool of reps
  MessageManager* mMessageManager; ///< takes messages intended for the user
};
}//namespace cx
#endif /* CXVIEW3D2_H_ */
