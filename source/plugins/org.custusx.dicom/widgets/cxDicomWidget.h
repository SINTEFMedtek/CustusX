/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDICOMWIDGET_H_
#define CXDICOMWIDGET_H_

#include "org_custusx_dicom_Export.h"

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"
class QItemSelection;
//class ctkDICOMBrowser;
class ctkDICOMAppWidget;
class ctkPluginContext;
class ctkDICOMDatabase;

namespace cx
{
class DICOMAppWidget;
typedef boost::shared_ptr<class Image> ImagePtr;

/**
 * Widget for dicom interaction
 *
 * \ingroup org_custusx_dicom
 *
 * \date 2014-05-02
 * \author Christian Askeland
 */
class org_custusx_dicom_EXPORT DicomWidget : public BaseWidget
{
	Q_OBJECT
public:
	DicomWidget(ctkPluginContext* context, QWidget* parent = 0);
	virtual ~DicomWidget();

    QString getDICOMDatabaseDirectory();
protected:
	virtual void prePaintEvent();
	ctkDICOMDatabase* getDatabase() const;

private slots:
    void onViewHeader();
    void onImportIntoCustusXAction();
    void deleteDICOMDB();
	void toggleDetailsSlot();

private:
    virtual QSize sizeHint () const { return QSize(600, 100);};///< Define a recommended size
	QVBoxLayout*  mVerticalLayout; ///< vertical layout is used
	DICOMAppWidget* mBrowser;
	ctkPluginContext* mContext;
	QAction* mViewHeaderAction;
	QAction* mImportIntoCustusXAction;
	QAction* mDetailsAction;
	QString mDicomShowAdvancedSettingsString;
	void createUI();
	void setupDatabaseDirectory();
	void importSeries(QString seriesUid);
	void loadIntoPatientModel(ImagePtr image, QString seriesUid);
	QStringList currentSeriesSelection();
	void showOrHideDetails();
};

} /* namespace cx */

#endif /* CXDICOMWIDGET_H_ */
