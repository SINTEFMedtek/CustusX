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

#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include "org_custusx_core_view_Export.h"

#include <vector>
#include <QVariant>
#include <QObject>
#include "cxDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxViewGroupData.h"
#include "cxSyncedValue.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxImageAlgorithms.h"

typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */


typedef boost::shared_ptr<class DataViewPropertiesInteractor> DataViewPropertiesInteractorPtr;
/** Provide an action list for showing data in views.
  *
  */
class org_custusx_core_view_EXPORT DataViewPropertiesInteractor : public QObject
{
	Q_OBJECT
public:
	DataViewPropertiesInteractor(VisServicesPtr services, ViewGroupDataPtr groupData);
	void setDataViewProperties(DataViewProperties properties);

	template <class DATA>
	void addDataActionsOfType(QWidget* parent);

private slots:
	void dataActionSlot();
private:
	void addDataAction(QString uid, QWidget* parent);
	VisServicesPtr mServices;
	ViewGroupDataPtr mGroupData;
	DataViewProperties mProperties;

	QString mLastDataActionUid;
};

template <class DATA>
void DataViewPropertiesInteractor::addDataActionsOfType(QWidget* parent)
{
	//add actions to the actiongroups and the contextmenu
	std::vector< boost::shared_ptr<DATA> > sorted = sortOnGroupsAndAcquisitionTime(mServices->patient()->getDataOfType<DATA>());
	mLastDataActionUid = "________________________";
	for (typename std::vector< boost::shared_ptr<DATA> >::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
	{
		this->addDataAction((*iter)->getUid(), parent);
	}
}


/**
 * \brief Superclass for ViewWrappers.
 *
 *
 * \date 6. apr. 2010
 * \\author jbake
 */
class org_custusx_core_view_EXPORT ViewWrapper: public QObject
{
Q_OBJECT
public:
	virtual ~ViewWrapper() {}
	virtual void initializePlane(PLANE_TYPE plane) {}
	virtual ViewPtr getView() = 0;
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy) = 0;
	virtual void setViewGroup(ViewGroupDataPtr group);
	virtual void updateView();

signals:
	void orientationChanged(ORIENTATION_TYPE type);

protected slots:
	void contextMenuSlot(const QPoint& point);
	void settingsChangedSlot(QString key);

	virtual void dataViewPropertiesChangedSlot(QString uid) = 0;
	virtual void videoSourceChangedSlot(QString uid) {}

protected:
	ViewWrapper(VisServicesPtr backend);

	void connectContextMenu(ViewPtr view);
	virtual QString getDataDescription() = 0;
	virtual QString getViewDescription() = 0;
	virtual void appendToContextMenu(QMenu& contextMenu) = 0;
	QStringList getAllDataNames(DataViewProperties properties) const;

	ViewGroupDataPtr mGroupData;
	VisServicesPtr mServices;
	DataViewPropertiesInteractorPtr mDataViewPropertiesInteractor;
	DataViewPropertiesInteractorPtr mShow3DSlicesInteractor;

	virtual void addReps();
private:
	DisplayTextRepPtr mPlaneTypeText;
	DisplayTextRepPtr mDataNameText;
};

/**
 * @}
 */
} //namespace cx

#endif /* CXVIEWWRAPPER_H_ */
