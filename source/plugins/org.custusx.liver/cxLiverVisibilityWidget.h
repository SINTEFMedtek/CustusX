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

namespace cx
{

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
 * (liver, pancreas, vessels, lesions, Couinaud segments) in the 2D/3D views.
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

private slots:
	void refreshStructures();

private:
	void addStructureButton(ORGAN_TYPE organType, QString label, QGridLayout* layout, int row);
	void toggleStructure(ORGAN_TYPE organType);
	void updateButtonColor(ORGAN_TYPE organType);
	void showMesh(MeshPtr mesh);
	void hideMesh(MeshPtr mesh);

	VisServicesPtr mServices;
	QMap<ORGAN_TYPE, SelectableLiverStructure> mStructures;
};

} /* namespace cx */

#endif /* CXLIVERVISIBILITYWIDGET_H_ */
