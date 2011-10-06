#ifndef VMORIENTATIONANNOTATIONREP_H_
#define VMORIENTATIONANNOTATIONREP_H_

#include "sscRepImpl.h"
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include <vtkCornerAnnotation.h>

namespace ssc
{

class OrientationAnnotation : public vtkCornerAnnotation
{
  vtkTypeMacro(OrientationAnnotation, vtkCornerAnnotation);
public:
  static OrientationAnnotation* New();
  OrientationAnnotation();
  ~OrientationAnnotation();
  virtual void SetTextActorsPosition(int vsize[2]);
  virtual void SetTextActorsJustification();
};

typedef vtkSmartPointer<class OrientationAnnotation> OrientationAnnotationPtr;


class OrientationAnnotationRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  static OrientationAnnotationRepPtr  New(const QString& uid,const QString& name);
  virtual ~OrientationAnnotationRep();
  virtual QString getType() const { return "vm::OrientationAnnotationRep"; };

  void setPlaneType( PLANE_TYPE type);
  void setVisible(bool visible);
private slots:
  void clinicalApplicationChangedSlot();
protected:
  OrientationAnnotationRep(const QString& uid, const QString& name);
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

  void setPlaneTypeNeurology(PLANE_TYPE type);
  void setPlaneTypeRadiology(PLANE_TYPE type);
  void createAnnotation();

  OrientationAnnotationPtr mOrientation;
  PLANE_TYPE mPlane;

  QString mNorthAnnotation;
  QString mSouthAnnotation;
  QString mEastAnnotation;
  QString mWestAnnotation;
//  ssc::TextDisplayPtr mNorth;
//  ssc::TextDisplayPtr mSouth;
//  ssc::TextDisplayPtr mWest;
//  ssc::TextDisplayPtr mEast;
};


}

#endif /*VMORIENTATIONANNOTATIONREP_H_*/

