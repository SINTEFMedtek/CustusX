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

private slots:
    void onViewHeader();
    void onImportIntoCustusXAction();
    void deleteDICOMDB();

private:
	QVBoxLayout*  mVerticalLayout; ///< vertical layout is used
	DICOMAppWidget* mBrowser;
	ctkPluginContext* mContext;
	QAction* mViewHeaderAction;
	QAction* mImportIntoCustusXAction;
	void createUI();
	void setupDatabaseDirectory();
	void importSeries(QString seriesUid);
	void loadIntoPatientModel(ImagePtr image, QString seriesUid);
	QStringList currentSeriesSelection();
	ctkDICOMDatabase* getDatabase() const;
};

} /* namespace cx */

#endif /* CXDICOMWIDGET_H_ */
