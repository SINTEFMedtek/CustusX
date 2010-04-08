#ifndef VMSLICEPLANES3DREP_H_
#define VMSLICEPLANES3DREP_H_

#include <vtkSmartPointer.h>
#include <vtkVectorText.h>
#include <vtkTextActor3D.h>
#include "sscRepImpl.h"
#include "sscSliceProxy.h"
#include "sscVtkHelperClasses.h"

typedef vtkSmartPointer<vtkVectorText> vtkVectorTextPtr;
typedef vtkSmartPointer<vtkTextActor3D> vtkTextActor3DPtr;

namespace ssc
{

/**Enumerates the contents of a view area.
 */
enum VIEW_AREA_TYPE
{
        vaSAGITTAL=0,
        vaCORONAL,
        vaAXIAL,
        vaANYPLANE,
        vaANYSIDEPLANE,
        vaANYRADIALPLANE,
        vaULTRASOUND,
        va3D,
	vaTEXTBOX,
        vaDUMMY,
        vaCOUNT
};

typedef boost::shared_ptr<class Rect3D> Rect3DPtr;
//typedef boost::shared_ptr<class Axes3D> Axes3DPtr;

class Rect3D
{
public:
	Rect3D( vtkRendererPtr renderer, const Vector3D& color);
	~Rect3D() {}
	void updatePosition( const DoubleBoundingBox3D bb, const Transform3D& M);	
private:
	ssc::GraphicalLine3D a,b,c,d;//,x;
};

/**Helper for the reps displaying slice planes in 3D and
 * markers for the planes in 2D. 
 */
class SlicePlanesProxy : public QObject
{
	Q_OBJECT
public:	
	SlicePlanesProxy();
	
	struct DataType
	{
		DoubleBoundingBox3D vp_s;
		ssc::SliceProxyPtr mSliceProxy;
		Vector3D mColor;
		Vector3D mPointPos_normvp;
	};
	typedef std::map<ssc::VIEW_AREA_TYPE, DataType> DataMap;

	void setViewportData(ssc::VIEW_AREA_TYPE type, ssc::SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s);
	DataMap getData();
		
signals:
	void changed();
private slots:
	void sliceProxyChanged();
private:
	DataMap mData;
	std::vector<Vector3D> mColors;	
};
typedef boost::shared_ptr<class SlicePlanesProxy> SlicePlanesProxyPtr;


typedef boost::shared_ptr<class SlicePlanes3DRep> SlicePlanes3DRepPtr;

/**Helper for displaying the position of 2D slice planes in the 3D view.
 */
class SlicePlanes3DRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	static SlicePlanes3DRepPtr New(const std::string& uid, const std::string& name="");
	virtual ~SlicePlanes3DRep();
	virtual std::string getType() const { return "ssc::SlicePlanes3DRep"; }
	void setProxy(SlicePlanesProxyPtr proxy);
	
protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:
	void changedSlot();
private:
	SlicePlanes3DRep(const std::string& uid, const std::string& name="");
	SlicePlanesProxyPtr mProxy;
	
	struct DataType
	{
		vtkTextActor3DPtr mText;
		ssc::GraphicalPoint3DPtr mPoint;
		Rect3DPtr mRect;
		ssc::Axes3DPtr mAxes;
	};
	typedef std::map<ssc::VIEW_AREA_TYPE, DataType> DataMap;
	DataMap mData;
};

typedef boost::shared_ptr<class SlicePlanes3DMarkerIn2DRep> SlicePlanes3DMarkerIn2DRepPtr;

/**Helper for displaying the position of 2D slice planes in the 3D view.
 */
class SlicePlanes3DMarkerIn2DRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	static SlicePlanes3DMarkerIn2DRepPtr New(const std::string& uid, const std::string& name="");
	virtual ~SlicePlanes3DMarkerIn2DRep();
	virtual std::string getType() const { return "ssc::SlicePlanes3DMarkerIn2DRep"; }
	void setProxy(ssc::VIEW_AREA_TYPE type, SlicePlanesProxyPtr proxy);
	
protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:
	void changedSlot();
private:
	SlicePlanes3DMarkerIn2DRep(const std::string& uid, const std::string& name="");
	SlicePlanesProxyPtr mProxy;
	//ssc::GraphicalPoint3DPtr mPoint;
	//OffsetPointPtr mPoint;
	ssc::TextDisplayPtr mText;
	ssc::VIEW_AREA_TYPE mType;
};

}

#endif /*VMSLICEPLANES3DREP_H_*/
