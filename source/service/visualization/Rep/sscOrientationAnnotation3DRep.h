/*
 * sscOrientationAnnotation3DRep.h
 *
 *  Created on: Mar 24, 2011
 *      Author: christiana
 */
#ifndef SSCORIENTATIONANNOTATION3DREP_H_
#define SSCORIENTATIONANNOTATION3DREP_H_

#include "sscRepImpl.h"
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"

typedef vtkSmartPointer<class vtkOrientationMarkerWidget> vtkOrientationMarkerWidgetPtr;
typedef vtkSmartPointer<class vtkAnnotatedCubeActor> vtkAnnotatedCubeActorPtr;
typedef vtkSmartPointer<class vtkProp> vtkPropPtr;

namespace ssc
{

typedef boost::shared_ptr<class OrientationAnnotation3DRep> OrientationAnnotation3DRepPtr;

/** Class for display of an orientation annotation cube in 3D.
 *
 */
class OrientationAnnotation3DRep: public ssc::RepImpl
{
Q_OBJECT
public:
  static OrientationAnnotation3DRepPtr New(const QString& uid, const QString& name);
  virtual ~OrientationAnnotation3DRep();
  virtual QString getType() const { return "ssc::OrientationAnnotation3DRep"; }

  bool getVisible() const;
  void setVisible(bool on);
  void setMarkerFilename(const QString filename); ///< empty and default means a cube
  void setSize(double size); ///< fraction of viewport to use

private slots:
protected:
  OrientationAnnotation3DRep(const QString& uid, const QString& name);
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

private:
  vtkOrientationMarkerWidgetPtr mMarker;
  double mSize;
  Vector3D mColor;
  static std::pair<QString, vtkPropPtr> mMarkerCache; ///< cache all loaded markers in order to save load time.

  void rebuild(vtkRenderWindowInteractorPtr interactor);
  vtkAxesActorPtr createAxes();
  vtkAnnotatedCubeActorPtr createCube();
  vtkPropPtr readMarkerFromFile(const QString filename);
  void reduceSTLFile(const QString source, const QString dest, double reduction);
};

}

#endif /* SSCORIENTATIONANNOTATION3DREP_H_ */
