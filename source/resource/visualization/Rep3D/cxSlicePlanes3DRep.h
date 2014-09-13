/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#ifndef CXSLICEPLANES3DREP_H_
#define CXSLICEPLANES3DREP_H_

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
class DataManager;

/** \brief Helper class for managing a set of slice planes.
 *
 * Use this to manage a set of slice planes
 * (represented by SliceProxy) that are used as a group.
 * The attributes are designed to work with SlicePlanes3DRep
 * and SlicePlanes3DMarkerIn2DRep.
 *
 * \sa SlicePlanes3DRep SlicePlanes3DMarkerIn2DRep
 *
 * \ingroup cx_resource_visualization
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
	void addSimpleSlicePlane(PLANE_TYPE type, DataServicePtr dataManager);
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
 * \ingroup cx_resource_visualization

 * \ingroup cx_resource_visualization_rep3D
 */
class SlicePlanes3DRep : public RepImpl
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
 * \ingroup cx_resource_visualization

 * \ingroup cx_resource_visualization_rep2D
 */
class SlicePlanes3DMarkerIn2DRep : public RepImpl
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
