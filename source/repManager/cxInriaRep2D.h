#ifndef CXINRIAREP2D_H_
#define CXINRIAREP2D_H_

#include "sscRepImpl.h"

#include <vtkViewImage2D.h>
#include "sscTransform3D.h"

/**
 * \class cxInriaRep2D
 *
 * \brief This class extends a vtkViewImage2D and make it adhere to the
 * ssc::Rep interface. It is both a Representation and a
 * vtkViewImage2D.
 *
 * \date Dec 8, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class vtkMetaDataSet;
namespace ssc
{
class View;
}
namespace cx
{
class Tool;

class InriaRep2D : public ssc::RepImpl
{
  typedef ssc::Transform3D Transform3D;
  typedef vtkSmartPointer<class vtkViewImage2D> vtkViewImage2DPtr;

  Q_OBJECT
public:
  InriaRep2D(const std::string& uid, const std::string& name="");
  ~InriaRep2D();

  virtual std::string getType() const;
  virtual void connectToView(ssc::View *theView);
  virtual void disconnectFromView(ssc::View *theView);
  virtual void setDataset(vtkMetaDataSet *dataset);

  vtkViewImage2DPtr getVtkViewImage2D();
  void setTool(Tool* tool); ///< connect to a tool
  void removeTool(Tool* tool); ///< remove connection to a tool
  bool hasTool(Tool* tool); ///< check if a rep is connected to a specific tool

protected:
  vtkImageData* getImageDataFromVtkMetaDataSet(vtkMetaDataSet *dataset);
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

  std::string       mType;    ///< the reps type as a string
  Tool*             mTool;    ///< the tool that controls the rep
  vtkViewImage2DPtr mInria;   ///< the inria object

private slots:
  void receiveToolTransformAndTimeStamp(Transform3D matrix, double timestamp);
  void receiveToolVisible(bool visible);

private:
  InriaRep2D();
};
typedef boost::shared_ptr<InriaRep2D> InriaRep2DPtr;
}//namespace cx
#endif /* CXINRIAREP2D_H_ */
