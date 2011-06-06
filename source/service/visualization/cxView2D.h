#ifndef CXVIEW2D_H_
#define CXVIEW2D_H_

#include "sscView.h"

class QWidget;
class QMenu;

namespace cx
{
class DataManager;
class RepManager;
class MessageManager;
class ViewManager;

/**
 * \class View2D
 *
 * \brief Class for displaying 2D representations.
 *
 * \date Dec 9, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class View2D : public ssc::View
{
  Q_OBJECT
public:
  View2D(const QString& uid, const QString& name="", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
  virtual ~View2D(); ///< empty

  virtual Type getType() const { return VIEW_2D;}; ///< get the class type
};
}//namespace cx
#endif /* CXVIEW2D2_H_ */
