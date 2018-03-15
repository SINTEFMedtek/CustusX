/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXSLICEPLANES3DREP_H_
#define CXSLICEPLANES3DREP_H_

#include "cxResourceVisualizationExport.h"

#include <map>
#include "cxRepImpl.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxViewportListener.h"
#include <QColor>

namespace cx
{

/** \brief Helper class for managing a set of slice planes.
 *
 * Use this to manage a set of slice planes
 * (represented by SliceProxy) that are used as a group.
 * The attributes are designed to work with SlicePlanes3DRep
 * and SlicePlanes3DMarkerIn2DRep.
 *
 * \sa SlicePlanes3DRep SlicePlanes3DMarkerIn2DRep
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT SlicePlanesProxy : public QObject
{
	Q_OBJECT
public:
	SlicePlanesProxy();

	/**Modifiable properties for one plane
	 *
	 */
	struct PropertiesType
	{
		std::map<PLANE_TYPE, QColor> mColor; /// normalized RGB
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
		SliceProxyPtr mSliceProxy;
		QString mSymbol;
		QColor mColor;
		Vector3D mPointPos_normvp;
	};
	typedef std::map<PLANE_TYPE, DataType> DataMap;

	void clearViewports();
	void setViewportData(PLANE_TYPE type, SliceProxyPtr slice, const DoubleBoundingBox3D& vp_s);
	void addSimpleSlicePlane(PLANE_TYPE type, PatientModelServicePtr dataManager);
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
 * \ingroup cx_resource_view

 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT SlicePlanes3DRep : public RepImpl
{
	Q_OBJECT
public:
	static SlicePlanes3DRepPtr New(const QString& uid="");
	virtual ~SlicePlanes3DRep();
	virtual QString getType() const { return "SlicePlanes3DRep"; }
	void setProxy(SlicePlanesProxyPtr proxy);
	SlicePlanesProxyPtr getProxy() { return mProxy; }
	void setDynamicLabelSize(bool on);

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

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

	SlicePlanes3DRep();
	void clearActors();
	SlicePlanesProxyPtr mProxy;
	ViewportListenerPtr mViewportListener;
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
 * \ingroup cx_resource_view

 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT SlicePlanes3DMarkerIn2DRep : public RepImpl
{
	Q_OBJECT
public:
	static SlicePlanes3DMarkerIn2DRepPtr New(const QString& uid="");
	virtual ~SlicePlanes3DMarkerIn2DRep();
	virtual QString getType() const { return "SlicePlanes3DMarkerIn2DRep"; }
	void setProxy(PLANE_TYPE type, SlicePlanesProxyPtr proxy);
	SlicePlanesProxyPtr getProxy() { return mProxy; }

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
private slots:
	void changedSlot();
private:
	SlicePlanes3DMarkerIn2DRep();
	SlicePlanesProxyPtr mProxy;
	TextDisplayPtr mText;
	PLANE_TYPE mType;
};

}

#endif /*CXSLICEPLANES3DREP_H_*/
