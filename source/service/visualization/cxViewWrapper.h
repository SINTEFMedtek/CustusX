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

#ifndef CXVIEWWRAPPER_H_
#define CXVIEWWRAPPER_H_

#include <vector>
#include <QVariant>
#include <QObject>
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxViewGroupData.h"

typedef vtkSmartPointer<class vtkPolyDataAlgorithm> vtkPolyDataAlgorithmPtr;
class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;

/**
 * \file
 * \addtogroup cx_service_visualization
 * @{
 */

///**Define a priority for the input data.
// * High means display on top, low means in the back.
// */
//int getPriority(DataPtr data);

///**Sorts DataPtr in default display ordering, using getPriority().
// */
//bool dataTypeSort(const DataPtr data1, const DataPtr data2);

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;

class SyncedValue: public QObject
{
Q_OBJECT
public:
	SyncedValue(QVariant val = QVariant());
	static SyncedValuePtr create(QVariant val = QVariant());
	void set(QVariant val);
	QVariant get() const;
	template<class T>
	T value() const { return this->get().value<T>(); }
private:
	QVariant mValue;
signals:
	void changed();
};

typedef boost::shared_ptr<class DataViewPropertiesInteractor> DataViewPropertiesInteractorPtr;
/** Provide an action list for showing data in views.
  *
  */
class DataViewPropertiesInteractor : public QObject
{
	Q_OBJECT
public:
	DataViewPropertiesInteractor(VisualizationServiceBackendPtr backend, ViewGroupDataPtr groupData);
	void addDataActions(QWidget* parent);
	void setDataViewProperties(DataViewProperties properties);

private slots:
	void dataActionSlot();
private:
	void addDataAction(QString uid, QWidget* parent);
	VisualizationServiceBackendPtr mBackend;
	ViewGroupDataPtr mGroupData;
	DataViewProperties mProperties;

	QString mLastDataActionUid;
};

/**
 * \brief Superclass for ViewWrappers.
 *
 *
 * \date 6. apr. 2010
 * \\author jbake
 */
class ViewWrapper: public QObject
{
Q_OBJECT
public:
	virtual ~ViewWrapper() {}
	virtual void initializePlane(PLANE_TYPE plane) {}
	virtual ViewWidget* getView() = 0;
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy) = 0;
	virtual void setViewGroup(ViewGroupDataPtr group);

//	virtual void setZoom2D(SyncedValuePtr value) {}
	virtual void setOrientationMode(SyncedValuePtr value) {}

	virtual void updateView() = 0;

signals:
	void orientationChanged(ORIENTATION_TYPE type);

protected slots:
	void contextMenuSlot(const QPoint& point);

	virtual void dataViewPropertiesChangedSlot(QString uid);
//	void dataAddedSlot(QString uid);
//	void dataRemovedSlot(QString uid);
	virtual void videoSourceChangedSlot(QString uid) {}

protected:
	ViewWrapper(VisualizationServiceBackendPtr backend);
//	virtual void dataAdded(DataPtr data) = 0;
//	virtual void dataRemoved(const QString& uid) = 0;

	void connectContextMenu(ViewWidget* view);
	virtual void appendToContextMenu(QMenu& contextMenu) = 0;
	QStringList getAllDataNames(DataViewProperties properties) const;

	ViewGroupDataPtr mGroupData;
	VisualizationServiceBackendPtr mBackend;
	DataViewPropertiesInteractorPtr mDataViewPropertiesInteractor;
	DataViewPropertiesInteractorPtr mShow3DSlicesInteractor;


};

/**
 * @}
 */
} //namespace cx

#endif /* CXVIEWWRAPPER_H_ */
