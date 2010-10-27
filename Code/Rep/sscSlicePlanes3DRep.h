#ifndef VMSLICEPLANES3DREP_H_
#define VMSLICEPLANES3DREP_H_

#include <map>
#include "sscRepImpl.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
/**Helper for the reps displaying slice planes in 3D and
 * markers for the planes in 2D. 
 */
class SlicePlanesProxy : public QObject
{
	Q_OBJECT
public:	
	SlicePlanesProxy();
	
	/**Modifiable properties for one plane
	 *
	 */
	struct PropertiesType
	{
	  std::map<PLANE_TYPE, Vector3D> mColor; /// normalized RGB
	  std::map<PLANE_TYPE, QString> mSymbol;
	  int m2DFontSize;
    int m3DFontSize;
    Vector3D mPointPos_normvp; ///< position of symbol in normalized space <0..1, 0..1>
    PLANE_TYPE mClipPlane; ///< what plane to use for 3D clipping
    int mLineWidth; ///< draw wireframe lines. 0 means no line
	};

	struct DataType
	{
		DoubleBoundingBox3D vp_s;
		ssc::SliceProxyPtr mSliceProxy;
		QString mSymbol;
		Vector3D mColor;
		Vector3D mPointPos_normvp;
	};
	typedef std::map<PLANE_TYPE, DataType> DataMap;

	void clearViewports();
	void setViewportData(PLANE_TYPE type, ssc::SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s);
	DataMap getData();

	void setVisible(bool visible);
	bool getVisible() const;
	void setDrawPlanes(bool on);
	bool getDrawPlanes() const;
	PropertiesType& getProperties() { return mProperties; } // use this to set properties BEFORE attaching reps/calling setviewportdata()
		
signals:
	void changed();
private:
	PropertiesType mProperties;
	bool mVisible;
  bool mDrawPlane; ///< turn opaque plane drawing on/off
	DataMap mData;
//	std::vector<Vector3D> mColors;
};
typedef boost::shared_ptr<class SlicePlanesProxy> SlicePlanesProxyPtr;


typedef boost::shared_ptr<class SlicePlanes3DRep> SlicePlanes3DRepPtr;

/**Helper for displaying the position of 2D slice planes in the 3D view.
 */
class SlicePlanes3DRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	static SlicePlanes3DRepPtr New(const QString& uid, const QString& name="");
	virtual ~SlicePlanes3DRep();
	virtual QString getType() const { return "ssc::SlicePlanes3DRep"; }
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

	 SlicePlanes3DRep(const QString& uid, const QString& name="");
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
	static SlicePlanes3DMarkerIn2DRepPtr New(const QString& uid, const QString& name="");
	virtual ~SlicePlanes3DMarkerIn2DRep();
	virtual QString getType() const { return "ssc::SlicePlanes3DMarkerIn2DRep"; }
	void setProxy(PLANE_TYPE type, SlicePlanesProxyPtr proxy);
	SlicePlanesProxyPtr getProxy() { return mProxy; }
	
protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
private slots:
	void changedSlot();
private:
	SlicePlanes3DMarkerIn2DRep(const QString& uid, const QString& name="");
	SlicePlanesProxyPtr mProxy;
	//ssc::GraphicalPoint3DPtr mPoint;
	//OffsetPointPtr mPoint;
	ssc::TextDisplayPtr mText;
	PLANE_TYPE mType;
};

}

#endif /*VMSLICEPLANES3DREP_H_*/
