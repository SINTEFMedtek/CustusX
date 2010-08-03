#ifndef VMORIENTATIONANNOTATIONREP_H_
#define VMORIENTATIONANNOTATIONREP_H_

#include "sscRepImpl.h"
#include "sscDefinitions.h"
#include "vtkSmartPointer.h"

namespace ssc
{
// forward declarations
typedef vtkSmartPointer<class OrientationAnnotation> OrientationAnnotationPtr;
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;

typedef  boost::shared_ptr<class OrientationAnnotationRep> OrientationAnnotationRepPtr;

class OrientationAnnotationRep : public ssc::RepImpl
{
public:
	static OrientationAnnotationRepPtr  New(const std::string& uid,const std::string& name);
	virtual ~OrientationAnnotationRep();
	virtual std::string getType() const { return "vm::OrientationAnnotationRep"; };
	
	void setPlaneType( PLANE_TYPE type);
	
protected:
	OrientationAnnotationRep(const std::string& uid, const std::string& name);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	
  void setPlaneTypeNeurology(PLANE_TYPE type);
  void setPlaneTypeRadiology(PLANE_TYPE type);
	void createAnnotation();
	
	OrientationAnnotationPtr mOrientation;

	std::string mNorthAnnotation;
	std::string mSouthAnnotation;
	std::string mEastAnnotation;
	std::string mWestAnnotation;
	ssc::TextDisplayPtr mNorth;
	ssc::TextDisplayPtr mSouth;
	ssc::TextDisplayPtr mWest;
	ssc::TextDisplayPtr mEast;
};


}

#endif /*VMORIENTATIONANNOTATIONREP_H_*/
