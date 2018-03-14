/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWGROUP_H_
#define CXVIEWGROUP_H_

#include "org_custusx_core_view_Export.h"

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
typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */


/**
 * \brief
 *
 * \date 18. mars 2010
 * \\author jbake
 */
class org_custusx_core_view_EXPORT ViewGroup: public QObject
{
	Q_OBJECT
public:
	explicit ViewGroup(CoreServicesPtr backend, QString uid);
	virtual ~ViewGroup();

	void addView(ViewWrapperPtr wrapper, SharedOpenGLContextPtr sharedOpenGLContext);
	void removeViews();
	ViewWrapperPtr getViewWrapperFromViewUid(QString viewUid);
	std::vector<ViewWrapperPtr> getWrappers() const { return mViewWrappers; }
	std::vector<ViewPtr> getViews() const;
	ViewGroupDataPtr getData() { return mViewGroupData; }
	virtual void addXml(QDomNode& dataNode); ///< store internal state info in dataNode
	virtual void parseXml(QDomNode dataNode); ///< load internal state info from dataNode
	void clearPatientData();
	CameraStylePtr getCameraStyle() { return mCameraStyle; }

	bool contains3DView() const;
	void initializeActiveView(SyncedValuePtr val);

protected slots:
//	void activateManualToolSlot();
	void mouseClickInViewGroupSlot();

protected:
	std::vector<ViewPtr> mViews;

	ViewGroupDataPtr mViewGroupData;
	std::vector<ViewWrapperPtr> mViewWrappers;
	CameraStylePtr mCameraStyle;
	CoreServicesPtr mBackend;
	SyncedValuePtr mActiveView;

private:
	template<class T>
	bool shouldUpdateActiveData(T activeData, std::vector<T> datas) const;
	void optionChangedSlot();
};

bool isViewWrapper2D(ViewWrapperPtr wrapper);

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWGROUP_H_ */
