#ifndef CXVIEW3D2_H_
#define CXVIEW3D2_H_

#include "sscView.h"
#include "sscTransform3D.h"
#include "sscTool.h"
#include "sscForwardDeclarations.h"

class QWidget;
class QMenu;

namespace cx
{

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
  View3D(const QString& uid, const QString& name="", QWidget *parent = NULL, Qt::WFlags f = 0); ///< constructor
  virtual ~View3D(); ///< empty
  virtual Type getType() const { return VIEW_3D;}; ///< get the class type

public slots:
protected slots:
protected:
};

}//namespace cx
#endif /* CXVIEW3D2_H_ */




