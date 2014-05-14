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

#ifndef CXDICOMWIDGET_H_
#define CXDICOMWIDGET_H_

#include "cxBaseWidget.h"
class ctkDICOMBrowser;

namespace cx
{

/**
 * Widget for dicom interaction
 *
 * \ingroup org_custusx_dicom
 *
 * \date 2014-05-02
 * \author Christian Askeland
 */
class DicomWidget : public BaseWidget
{
	Q_OBJECT
public:
	DicomWidget(QWidget* parent = 0);
	virtual ~DicomWidget();

	virtual QString defaultWhatsThis() const;

private slots:
	void onViewHeader();
	void onImportIntoCustusXAction();
protected:
	virtual void prePaintEvent();
private:
	QVBoxLayout*  mVerticalLayout; ///< vertical layout is used
	ctkDICOMBrowser* mBrowser;

	QAction* mViewHeaderAction;
	QAction* mImportIntoCustusXAction;
	void createUI();
	void setupDatabaseDirectory();
	void importSeries(QString seriesUid);
};

} /* namespace cx */

#endif /* CXDICOMWIDGET_H_ */
