// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef VMSLICEPLANES3DREP_H_
#define VMSLICEPLANES3DREP_H_

#include <map>
#include "sscRepImpl.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscViewportListener.h"

namespace ssc
{

/**\brief Helper class for managing a set of slice planes.
 *
 * Use this to manage a set of slice planes
 * (represented by SliceProxy) that are used as a group.
 * The attributes are designed to work with SlicePlanes3DRep
 * and SlicePlanes3DMarkerIn2DRep.
 *
 * \sa SlicePlanes3DRep SlicePlanes3DMarkerIn2DRep
 *
 * \ingroup sscProxy
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
	void addSimpleSlicePlane(PLANE_TYPE type);
	DataMap getData();

	void setVisible(bool visible);
	bool getVisible() const;
	void setDrawPlanes(bool on);
	bool getDrawPlanes() const;
	PropertiesType& getProperties() { return mProperties; } // use this to set properties BEFORE attaching reps/calling setviewportdata()

	void connectTo3D(bool on);

signals:
	void changed();
private:
	PropertiesType mProperties;
	bool mVisible;
	bool mDrawPlane; ///< turn opaque plane drawing on/off
	DataMap mData;
	bool mConnectedTo3D;
};
typedef boost::shared_ptr<class SlicePlanesProxy> SlicePlanesProxyPtr;


typedef boost::shared_ptr<class SlicePlanes3DRep> SlicePlanes3DRepPtr;

/**\brief Display a set of planes in 3D.
 *
 * Draw all the planes defined in a SlicesPlanesProxy using
 * the settings from SlicePlanesProxy. Only graphical plane
 * representations are drawn, no image data.
 *
 * \sa SlicePlanesProxy SlicePlanes3DMarkerIn2DRep
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
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
	void setDynamicLabelSize(bool on);

protected:
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

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

	SlicePlanes3DRep(const QString& uid, const QString& name = "");
	void clearActors();
	SlicePlanesProxyPtr mProxy;
	ssc::View *mView;
	ssc::ViewportListenerPtr mViewportListener;
	void rescale();
};

typedef boost::shared_ptr<class SlicePlanes3DMarkerIn2DRep> SlicePlanes3DMarkerIn2DRepPtr;

/**\brief Display annotations for the SlicePlanesProxy planes in 2D.
 *
 * When several 2D View are drawn in a 3D View using a SlicePlanes3DRep,
 * the SlicePlanes3DMarkerIn2DRep can be used to annotate the 2D View with
 * the same annotation as is shown in the 3D View.
 *
 * \sa SlicePlanesProxy SlicePlanes3DRep
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
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
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);
private slots:
	void changedSlot();
private:
	SlicePlanes3DMarkerIn2DRep(const QString& uid, const QString& name="");
	SlicePlanesProxyPtr mProxy;
	ssc::TextDisplayPtr mText;
	PLANE_TYPE mType;
};

}

#endif /*VMSLICEPLANES3DREP_H_*/
