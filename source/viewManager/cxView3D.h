#ifndef CXVIEW3D_H_
#define CXVIEW32_H_

#include "sscView.h"

/**
 * cxView3D.h
 *
 * \brief
 *
 * \date Dec 9, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
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

class View3D : public ssc::View
{
typedef ssc::DataManager DataManager;

public:
  View3D(const std::string& uid, const std::string& name="", QWidget *parent = NULL, Qt::WFlags f = 0);
  virtual ~View3D();

  virtual Type getType() const { return VIEW_3D;};
  void contextMenuEvent(QContextMenuEvent *event);

protected:
  QMenu* mContextMenu;
  QMenu* mMakeVolumetricRepMenu;
  QMenu* mMakeGeometricRepMenu;

  DataManager* mDataManager;
  RepManager* mRepManager;
  MessageManager& mMessageManager;
};
}//namespace cx
#endif /* CXVIEW3D2_H_ */
