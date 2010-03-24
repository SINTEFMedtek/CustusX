#ifndef VMORIENTATIONANNOTATIONREP_H_
#define VMORIENTATIONANNOTATIONREP_H_

#include "sscRepImpl.h"
//#include "vmDefinitions.h"
#include "sscSliceProxy.h"
#include <vtkCornerAnnotation.h>
#include "sscVtkHelperClasses.h"

namespace ssc
{
typedef  vtkSmartPointer<class OrientationAnnotation> OrientationAnnotationPtr;
typedef  boost::shared_ptr<class OrientationAnnotationRep> OrientationAnnotationRepPtr;

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

class OrientationAnnotationRep : public ssc::RepImpl
{
public:
	static OrientationAnnotationRepPtr  New(const std::string& uid,const std::string& name);
	virtual ~OrientationAnnotationRep();
	virtual std::string getType() const { return "vm::OrientationAnnotationRep"; };
	
	void setPlaneType( PLANE_TYPE type );
	
protected:
	OrientationAnnotationRep(const std::string& uid, const std::string& name);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	
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
