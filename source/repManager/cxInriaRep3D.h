#ifndef CXINRIAREP3D_H_
#define CXINRIAREP3D_H_

#include "sscRepImpl.h"

#include <vtkViewImage3D.h>
#include <vtkSmartPointer.h>
#include "sscTransform3D.h"

/**
 * \class cxInriaRep3D
 *
 * \brief This class extends a vtkViewImage3D and makes it adhere to the
 * ssc::Rep interface.
 *
 * \date Dec 8, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
namespace ssc
{
class View;
}
namespace cx
{
class InriaRep3D : public ssc::RepImpl
{
  typedef vtkSmartPointer<class vtkViewImage3D> vtkViewImage3DPtr;

  Q_OBJECT
public:
  InriaRep3D(const std::string& uid, const std::string& name="");
  ~InriaRep3D();

  virtual std::string getType() const;
  virtual void connectToView(ssc::View *theView);
  virtual void disconnectFromView(ssc::View *theView);
  vtkViewImage3DPtr getVtkViewImage3D();

//  void setTool(Tool* tool); ///< connect to a tool
//  void removeTool(Tool* tool); ///< remove connection to a tool
//  bool hasTool(Tool* tool); ///< check if a rep is connected to a specific tool

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

  std::string        mType;          ///< the reps type as a string
//  Tool*       mTool;          ///< the tool that controls the rep
  vtkViewImage3DPtr  mInria;   ///< the inria object

//private slots:
//  void receiveToolTransformAndTimeStamp(ssc::Transform3D matrix, double timestamp);
//  void receiveToolVisible(bool visible);

private:
  InriaRep3D();
};
typedef boost::shared_ptr<InriaRep3D> InriaRep3DPtr;
}//namespace cx
#endif /* CXINRIAREP3D_H_ */
