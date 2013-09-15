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

/**
 * \file
 * \addtogroup cxServiceVisualization
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
private:
	QVariant mValue;signals:
	void changed();
};

//typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

///** \brief Container for data shared between all members of a view group
// */
//class ViewGroupData: public QObject
//{
//Q_OBJECT
//public:
//	ViewGroupData();
//	void requestInitialize();
//	std::vector<DataPtr> getData() const;
//	QString getVideoSource() const;
//	void addData(DataPtr data);
//	void addDataSorted(DataPtr data); ///< add data in a predefined ordering: CT/MR/SC/US/USA/Mesh/Metrics
//	void setVideoSource(QString uid);
//	bool removeData(DataPtr data);
//	void clearData();
//	std::vector<ImagePtr> getImages() const;
//	std::vector<MeshPtr> getMeshes() const;

//	CameraDataPtr getCamera3D() { return mCamera3D; }

//	// view options for this group.
//	struct Options
//	{
//		Options();
//		bool mShowLandmarks;
//		bool mShowPointPickerProbe;
//		MeshPtr mPickerGlyph;
//	};

//	Options getOptions() const;
//	void setOptions(Options options);

//private slots:
//    void removeDataSlot(QString uid);

//signals:
//	void dataAdded(QString uid);
//	void dataRemoved(QString uid);
//	void videoSourceChanged(QString uid);
//	void initialized();
//	void optionsChanged();

//private:
//	QString mVideoSource;
//	std::vector<DataPtr> mData;
//	CameraDataPtr mCamera3D;
//	Options mOptions;
//};

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

	virtual void setZoom2D(SyncedValuePtr value) {}
	virtual void setOrientationMode(SyncedValuePtr value) {}

	virtual void updateView() = 0;

signals:
	void orientationChanged(ORIENTATION_TYPE type);

protected slots:
	void contextMenuSlot(const QPoint& point);
	void dataActionSlot();

	void dataAddedSlot(QString uid);
	void dataRemovedSlot(QString uid);
	virtual void videoSourceChangedSlot(QString uid) {}

protected:
	virtual void dataAdded(DataPtr data) = 0;
	virtual void dataRemoved(const QString& uid) = 0;

	void connectContextMenu(ViewWidget* view);
	virtual void appendToContextMenu(QMenu& contextMenu) = 0;
	void addDataAction(QString uid, QMenu* contextMenu);
	QStringList getAllDataNames() const;

	ViewGroupDataPtr mGroupData;

private:
	QString mLastDataActionUid;

};

/**
 * @}
 */
} //namespace cx

#endif /* CXVIEWWRAPPER_H_ */
