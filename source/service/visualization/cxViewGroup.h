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
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"

class QMenu;
class QPoint;

namespace cx
{
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;
typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class CameraStyle> CameraStylePtr;
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;
typedef boost::shared_ptr<class Navigation> NavigationPtr;

/**
 * \file
 * \addtogroup cx_service_visualization
 * @{
 */


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
	CameraStylePtr getCameraStyle() { return mCameraStyle; }

	bool contains3DView() const;
	void syncOrientationMode(SyncedValuePtr val);
	void initializeActiveView(SyncedValuePtr val);

private slots:
	void activateManualToolSlot();
	void mouseClickInViewGroupSlot();

protected:
	std::vector<ViewWidgetQPtr> mViews;

	ViewGroupDataPtr mViewGroupData;
	std::vector<ViewWrapperPtr> mViewWrappers;
	CameraStylePtr mCameraStyle;
	VisualizationServiceBackendPtr mBackend;
	SyncedValuePtr mActiveView;
};

bool isViewWrapper2D(ViewWrapperPtr wrapper);

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWGROUP_H_ */
