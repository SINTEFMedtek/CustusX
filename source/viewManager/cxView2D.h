#ifndef CXVIEW2D_H_
#define CXVIEW2D_H_

#include "sscView.h"

/**
 * cxView2D2.h
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

class View2D : public ssc::View
{
typedef ssc::DataManager DataManager;

public:
  View2D(const std::string& uid, const std::string& name="", QWidget *parent = NULL, Qt::WFlags f = 0);
  virtual ~View2D();

  virtual Type getType() const { return VIEW_2D;};
  void contextMenuEvent(QContextMenuEvent *event);

protected:
  QMenu* mContextMenu;

  DataManager* mDataManager;
  RepManager* mRepManager;

};
}//namespace cx
#endif /* CXVIEW2D2_H_ */
