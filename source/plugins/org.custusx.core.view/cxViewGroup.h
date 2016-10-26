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
typedef boost::shared_ptr<class Navigation> NavigationPtr;

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

	void addView(ViewWrapperPtr wrapper);
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
	void setSharedOpenGLContext(SharedOpenGLContextPtr sharedOpenGLContext);
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
