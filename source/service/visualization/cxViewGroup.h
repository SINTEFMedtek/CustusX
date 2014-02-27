// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include <vector>
#include <QObject>
#include <QDomDocument>
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"
#include "sscVector3D.h"

class QMenu;
class QPoint;

namespace cx
{
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;
typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class CameraStyle> CameraStylePtr;
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

/**Helper for functions independent of state.
 * Not sure if we need this - think of better place.
 *
 */
class Navigation
{
public:
	Navigation(VisualizationServiceBackendPtr backend);
	void centerToData(DataPtr image);
	void centerToView(const std::vector<DataPtr>& images);
	void centerToGlobalDataCenter();
	void centerToTooltip();

private:
	VisualizationServiceBackendPtr mBackend;
	Vector3D findViewCenter(const std::vector<DataPtr>& images);
	Vector3D findGlobalDataCenter();
	Vector3D findDataCenter(std::vector<DataPtr> data);

	void centerManualTool(Vector3D& p_r);
};
typedef boost::shared_ptr<Navigation> NavigationPtr;

/**
 * \brief
 *
 * \date 18. mars 2010
 * \\author jbake
 */
class ViewGroup: public QObject
{
Q_OBJECT
public:
	explicit ViewGroup(VisualizationServiceBackendPtr backend);
	virtual ~ViewGroup();

	void addView(ViewWrapperPtr wrapper);
	void removeViews();
	ViewWrapperPtr getViewWrapperFromViewUid(QString viewUid);
	std::vector<ViewWrapperPtr> getWrappers() const { return mViewWrappers; }
	std::vector<ViewWidgetQPtr> getViews() const;
	ViewGroupDataPtr getData() { return mViewGroupData; }
	virtual void addXml(QDomNode& dataNode); ///< store internal state info in dataNode
	virtual void parseXml(QDomNode dataNode); ///< load internal state info from dataNode
	void clearPatientData();
	std::vector<ImagePtr> getImages();
	CameraStylePtr getCameraStyle() { return mCameraStyle; }

	bool contains3DView() const;
	void syncOrientationMode(SyncedValuePtr val);

public slots:

private slots:
	void activateManualToolSlot();
	void mouseClickInViewGroupSlot();

protected:
	//zoom2d is the same for all viewwrapper2ds in a viewgroup
//	void setZoom2D(double newZoom);

	std::vector<ViewWidgetQPtr> mViews;

//	struct SyncGroup
//	{
//		void activateGlobal(bool val)
//		{
//			if (val)
//				mActive = mGlobal;
//			else
//				mActive = mLocal;
//		}
//		SyncedValuePtr mGlobal;
//		SyncedValuePtr mLocal;
//		SyncedValuePtr mActive;
//	};
//	SyncGroup mZoom2D;

	ViewGroupDataPtr mViewGroupData;
	std::vector<ViewWrapperPtr> mViewWrappers;
//  SlicePlanesProxyPtr mSlicePlanesProxy;
	CameraStylePtr mCameraStyle;
	VisualizationServiceBackendPtr mBackend;
};

bool isViewWrapper2D(ViewWrapperPtr wrapper);

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWGROUP_H_ */
