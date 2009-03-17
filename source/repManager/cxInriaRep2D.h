#ifndef CXINRIAREP2D_H_
#define CXINRIAREP2D_H_

#include "sscRepImpl.h"

#include <vtkViewImage2D.h>
#include "sscTransform3D.h"

class vtkMetaDataSet;
typedef vtkSmartPointer<class vtkViewImage2D> vtkViewImage2DPtr;
typedef vtkSmartPointer<class vtkEventQtSlotConnect> vtkEventQtSlotConnectPtr;
namespace ssc
{
class View;
}
namespace cx
{
class Tool;

/**
 * \class InriaRep2D
 *
 * \brief This class extends a vtkViewImage2D and make it adhere to the
 * ssc::Rep interface. It is both a Representation and a
 * vtkViewImage2D.
 *
 * \date Dec 8, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class InriaRep2D : public ssc::RepImpl
{
  typedef ssc::Transform3D Transform3D;

  Q_OBJECT
public:
  InriaRep2D(const std::string& uid, const std::string& name=""); ///< constructor
  ~InriaRep2D(); ///< empty

  virtual std::string getType() const;
  virtual void connectToView(ssc::View *theView);
  virtual void disconnectFromView(ssc::View *theView);
  virtual void setDataset(vtkMetaDataSet *dataset);

  vtkViewImage2DPtr getVtkViewImage2D(); ///< returns the internal VtkViewImage2D
  void setTool(Tool* tool); ///< connect to a tool
  void removeTool(Tool* tool); ///< remove connection to a tool
  bool hasTool(Tool* tool); ///< check if a rep is connected to a specific tool

signals:
  void pointPicked(double x, double y, double z); ///< send out a signal every time current position changes

public slots:
  void pickSurfacePointSlot(vtkObject* object); ///< picks a point on the volumes surface
  void syncSetPosition(double x, double y, double z); ///< sets the current position

protected:
  vtkImageData* getImageDataFromVtkMetaDataSet(vtkMetaDataSet *dataset); ///< get vtkImageData
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

  std::string       mType;    ///< the reps type as a string
  Tool*             mTool;    ///< the tool that controls the rep
  vtkViewImage2DPtr mInria;   ///< the inria object

  vtkEventQtSlotConnectPtr mConnections; ///< used to sending signals og evnts between qt and vtk

private slots:
  void toolTransformAndTimeStampSlot(Transform3D matrix, double timestamp); ///< updates sync position
  void toolVisibleSlot(bool visible); ///< empty

private:
  InriaRep2D(); ///< not implemented
};
typedef boost::shared_ptr<InriaRep2D> InriaRep2DPtr;
}//namespace cx
#endif /* CXINRIAREP2D_H_ */
