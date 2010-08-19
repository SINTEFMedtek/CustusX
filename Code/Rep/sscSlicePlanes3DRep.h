#ifndef VMSLICEPLANES3DREP_H_
#define VMSLICEPLANES3DREP_H_

#include <map>
#include "sscRepImpl.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
#include "sscDefinitions.h"
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkVectorText> vtkVectorTextPtr;
typedef vtkSmartPointer<class vtkTextActor3D> vtkTextActor3DPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class GraphicalPoint3D> GraphicalPoint3DPtr;
typedef boost::shared_ptr<class Rect3D> Rect3DPtr;
typedef boost::shared_ptr<class Axes3D> Axes3DPtr;
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;

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
	typedef std::map<PLANE_TYPE, DataType> DataMap;

	void setViewportData(PLANE_TYPE type, ssc::SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s);
	DataMap getData();

	void setVisible(bool visible);
	bool getVisible() const;
		
signals:
	void changed();
private:
	bool mVisible;
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
  SlicePlanesProxyPtr getProxy() { return mProxy; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:
	void changedSlot();
private:
	struct DataType
	{
		vtkTextActor3DPtr mText;
		GraphicalPoint3DPtr mPoint;
		Rect3DPtr mRect;
		Axes3DPtr mAxes;
	};
	typedef std::map<PLANE_TYPE, DataType> DataMap;
	DataMap mData;

	 SlicePlanes3DRep(const std::string& uid, const std::string& name="");
	 void clearActors();
//	  void setVisibility(DataType data);
	  SlicePlanesProxyPtr mProxy;
	  ssc::View* mView;
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
	void setProxy(PLANE_TYPE type, SlicePlanesProxyPtr proxy);
	SlicePlanesProxyPtr getProxy() { return mProxy; }
	
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
	PLANE_TYPE mType;
};

}

#endif /*VMSLICEPLANES3DREP_H_*/
