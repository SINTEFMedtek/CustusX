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

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

/**Define a priority for the input data.
 * High means display on top, low means in the back.
 */
int getPriority(ssc::DataPtr data);

/**Sorts ssc::DataPtr in default display ordering, using getPriority().
 */
bool dataTypeSort(const ssc::DataPtr data1, const ssc::DataPtr data2);


typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

/** \brief Container for data shared between all members of a view group
 */
class ViewGroupData: public QObject
{
Q_OBJECT
public:
	ViewGroupData();
	void requestInitialize();
	std::vector<ssc::DataPtr> getData() const;
	QString getVideoSource() const;
	void addData(ssc::DataPtr data);
	void addDataSorted(ssc::DataPtr data); ///< add data in a predefined ordering: CT/MR/SC/US/USA/Mesh/Metrics
	void setVideoSource(QString uid);
	bool removeData(ssc::DataPtr data);
	void clearData();
	std::vector<ssc::ImagePtr> getImages() const;
	std::vector<ssc::MeshPtr> getMeshes() const;

	CameraDataPtr getCamera3D() { return mCamera3D; }

	// view options for this group.
	struct Options
	{
		Options();
		bool mShowLandmarks;
		bool mShowPointPickerProbe;
		ssc::MeshPtr mPickerGlyph;
	};

	Options getOptions() const;
	void setOptions(Options options);

private slots:
	void removeDataSlot(QString uid);

signals:
	void dataAdded(QString uid);
	void dataRemoved(QString uid);
	void videoSourceChanged(QString uid);
	void initialized();
	void optionsChanged();

private:
	QString mVideoSource;
	std::vector<ssc::DataPtr> mData;
	CameraDataPtr mCamera3D;
	Options mOptions;
};


/**
 * @}
 */
} //namespace cx

#endif // CXVIEWGROUPDATA_H
