#ifndef CXINRIAREP3D_H_
#define CXINRIAREP3D_H_

#include "sscRepImpl.h"

#include <vtkViewImage3D.h>
#include <vtkSmartPointer.h>
#include "sscTransform3D.h"

namespace ssc
{
class View;
}
namespace cx
{

/**
 * \class InriaRep3D
 *
 * \brief This class extends a vtkViewImage3D and makes it adhere to the
 * ssc::Rep interface.
 *
 * \date Dec 8, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class InriaRep3D : public ssc::RepImpl
{
  typedef vtkSmartPointer<class vtkViewImage3D> vtkViewImage3DPtr;

  Q_OBJECT
public:
  InriaRep3D(const QString& uid, const QString& name=""); ///< constructor
  ~InriaRep3D(); ///< empty

  virtual QString getType() const;
  virtual void connectToView(ssc::View *theView);
  virtual void disconnectFromView(ssc::View *theView);
  vtkViewImage3DPtr getVtkViewImage3D(); ///< returns the internal VtkViewImage3D

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view); ///<
  virtual void removeRepActorsFromViewRenderer(ssc::View* view); ///<

  QString        mType;          ///< the reps type as a string
  vtkViewImage3DPtr  mInria;   ///< the inria object

private:
  InriaRep3D();
};
typedef boost::shared_ptr<InriaRep3D> InriaRep3DPtr;
}//namespace cx
#endif /* CXINRIAREP3D_H_ */
