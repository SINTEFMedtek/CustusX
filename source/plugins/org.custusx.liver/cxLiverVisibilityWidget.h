/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLIVERVISIBILITYWIDGET_H_
#define CXLIVERVISIBILITYWIDGET_H_

#include "org_custusx_liver_Export.h"
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include <QMap>

class QPushButton;
class QGridLayout;
class QComboBox;

namespace cx
{

typedef boost::shared_ptr<class StringPropertyActiveImage> StringPropertyActiveImagePtr;

struct org_custusx_liver_EXPORT SelectableLiverStructure
{
	QString mName;
	QPushButton* mButton;
	bool mViewEnabled;
	MeshPtr mMesh;

	SelectableLiverStructure();
};

/**
 * Widget for toggling visibility of the segmented liver structures
 * (liver, pancreas, vessels, lesions, Couinaud segments) in the 2D/3D views,
 * scoped to one selected source image at a time (since running segmentation
 * against more than one volume can produce several meshes sharing the same
 * organ type, one per source image).
 *
 * \ingroup org_custusx_liver
 */
class org_custusx_liver_EXPORT LiverVisibilityWidget : public BaseWidget
{
	Q_OBJECT
public:
	explicit LiverVisibilityWidget(VisServicesPtr services, QWidget* parent = 0);
	virtual ~LiverVisibilityWidget();

	static QString getWidgetName();
	// Pure/stateless apart from the patient service - public so it can be
	// unit tested directly with a mock patient service and synthetic
	// parent-frame chains.
	static bool descendsFrom(PatientModelServicePtr patient, QString uid, QString ancestorUid);

private slots:
	void refreshStructures();
	void toggleAllSegments();
	void toggleSourceVolume();
	void rebuildViewGroupSelector();

private:
	ImagePtr sourceImage() const;
	void addStructureButton(ORGAN_TYPE organType, QString label, QGridLayout* layout, int row);
	void toggleStructure(ORGAN_TYPE organType);
	void updateButtonColor(ORGAN_TYPE organType);
	void updateAllSegmentsButtonColor();
	void updateSourceVolumeButtonColor();
	MeshPtr findMeshForSourceImage(ORGAN_TYPE organType, ImagePtr sourceImage) const;
	int selectedViewGroupIndex() const;
	bool isShown(QString uid) const;
	void showData(QString uid);
	void hideData(QString uid);

	VisServicesPtr mServices;
	StringPropertyActiveImagePtr mSourceImageSelector;
	QComboBox* mViewGroupSelector;
	QMap<ORGAN_TYPE, SelectableLiverStructure> mStructures;
	QList<ORGAN_TYPE> mSegmentOrganTypes;
	QPushButton* mAllSegmentsButton;
	bool mAllSegmentsViewEnabled;
	QPushButton* mSourceVolumeButton;
	bool mSourceVolumeViewEnabled;
};

} /* namespace cx */

#endif /* CXLIVERVISIBILITYWIDGET_H_ */
