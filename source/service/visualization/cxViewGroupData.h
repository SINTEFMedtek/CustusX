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
#ifndef CXVIEWGROUPDATA_H
#define CXVIEWGROUPDATA_H

#include <vector>
#include <QVariant>
#include <QObject>
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxPlaneTypeCollection.h"
class QDomNode;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */


/**Define a priority for the input data.
 * High means display on top, low means in the back.
 */
int getPriority(DataPtr data);

/**Sorts DataPtr in default display ordering, using getPriority().
 */
bool dataTypeSort(const DataPtr data1, const DataPtr data2);

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

/** Stores how a Data should be visualized in 2D and 3D views.
  */
class DataViewProperties
{
public:
	DataViewProperties() : mVolume3D(false), mSlice3D(false), mSlice2D(false) {}
	static DataViewProperties createDefault();
	static DataViewProperties createFull();
	static DataViewProperties createVolume3D();
	static DataViewProperties createSlice3D();
	static DataViewProperties createSlice2D();
	static DataViewProperties create3D();
	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode dataNode);
	bool empty() const;
	DataViewProperties addFlagsIn(DataViewProperties rhs) const;
	DataViewProperties removeFlagsIn(DataViewProperties rhs) const;
	bool containsAnyFlagsIn(DataViewProperties required) const;

	bool hasVolume3D() const { return mVolume3D; }
	bool hasSlice3D() const { return mSlice3D; }
	bool hasSlice2D() const { return mSlice2D; }

private:
	bool mVolume3D;
	bool mSlice3D;
	bool mSlice2D;
};

/** \brief Container for data shared between all members of a view group
 */
class ViewGroupData: public QObject
{
Q_OBJECT
public:
	explicit ViewGroupData(VisualizationServiceBackendPtr backend);
	void requestInitialize();
//	std::vector<DataPtr> getData() const;
	std::vector<DataPtr> getData(DataViewProperties properties=DataViewProperties::createFull()) const;
	QString getVideoSource() const;
	void addData(DataPtr data);
	void addDataSorted(DataPtr data); ///< add data in a predefined ordering: CT/MR/SC/US/USA/Mesh/Metrics
	void setVideoSource(QString uid);
	bool removeData(DataPtr data);
	void clearData();
	std::vector<ImagePtr> getImages(DataViewProperties properties) const;
	std::vector<MeshPtr> getMeshes(DataViewProperties properties) const;
	DataViewProperties getProperties(DataPtr data);
	void setProperties(DataPtr data, DataViewProperties properties);

	void initializeGlobal2DZoom(SyncedValuePtr val);
	SyncedValuePtr getGroup2DZoom();
	SyncedValuePtr getGlobal2DZoom();

	CameraDataPtr getCamera3D() { return mCamera3D; }

//	std::vector<ImagePtr> get3DSliceImages();
	PlaneTypeCollection getSliceDefinitions() { return mSliceDefinitions; }
	void setSliceDefinitions(PlaneTypeCollection val) { mSliceDefinitions = val; emit optionsChanged(); }

	// view options for this group.
	struct Options
	{
		Options();
		bool mShowLandmarks;
		bool mShowPointPickerProbe;
		MeshPtr mPickerGlyph;
	};

	Options getOptions() const;
	void setOptions(Options options);

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode dataNode);

private slots:
	void dataAddedOrRemovedInManager();

signals:
	void dataAdded(QString uid);
	void dataRemoved(QString uid);
	void videoSourceChanged(QString uid);
	void initialized();
	void optionsChanged();

private:
	VisualizationServiceBackendPtr mBackend;
	QString mVideoSource;
//	std::vector<DataPtr> mData;
	typedef std::pair<DataPtr, DataViewProperties> DataAndViewProperties;
	std::vector<DataAndViewProperties> mData;
	PlaneTypeCollection mSliceDefinitions;
	CameraDataPtr mCamera3D;
	Options mOptions;
	SyncedValuePtr mGroup2DZoom;
	SyncedValuePtr mGlobal2DZoom;

	struct data_equals
	{
		data_equals(DataPtr data) : mData(data) {}
		bool operator()(const DataAndViewProperties& right)
		{
			return mData == right.first;
		}
		DataPtr mData;
	};

	template<class DATA_TYPE>
	std::vector<boost::shared_ptr<DATA_TYPE> > getDataOfType(DataViewProperties requiredProperties) const;
	bool contains(DataPtr data) const;
};


/**
 * @}
 */
} //namespace cx

#endif // CXVIEWGROUPDATA_H
