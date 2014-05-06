/*
 * cxDicomWidget.h
 *
 *  Created on: May 2, 2014
 *      Author: christiana
 */

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
private:
	QVBoxLayout*  mVerticalLayout; ///< vertical layout is used
	ctkDICOMBrowser* mBrowser;

	QAction* mViewHeaderAction;
	QAction* mImportIntoCustusXAction;
	void setupDatabaseDirectory();
	void importSeries(QString seriesUid);
};

} /* namespace cx */

#endif /* CXDICOMWIDGET_H_ */
