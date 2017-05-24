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

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#undef REGISTERED
#include "ctkServiceTracker.h"
#include "ctkDICOMBrowser.h"
#include "ctkDICOMAppWidget.h"
#include "cxDICOMAppWidget.h"
#include "ctkDICOMTableManager.h"
#include "ctkDICOMObjectListWidget.h"
#include "ctkPluginContext.h"
#include "cxDicomWidget.h"
#include "cxProfile.h"
#include "cxTypeConversions.h"
#include "cxDicomConverter.h"
#include "cxLogger.h"

#include "cxPatientModelService.h"
#include "cxDicomImageReader.h"
#include "cxVisServices.h"
#include "cxViewService.h"
#include "cxSettings.h"

namespace cx
{

DicomWidget::DicomWidget(ctkPluginContext *context, QWidget *parent) :
	BaseWidget(parent, "dicom_widget", "DICOM"),
	mVerticalLayout(new QVBoxLayout(this)),
	mBrowser(NULL),
	mContext(context),
	mDicomShowAdvancedSettingsString("Dicom/ShowAdvanced")
{
	this->setModified();

}

DicomWidget::~DicomWidget()
{
    this->deleteDICOMDB();
}

void DicomWidget::prePaintEvent()
{
	if (!mBrowser)
	{
		this->createUI();
	}
}

void DicomWidget::createUI()
{
	if (mBrowser)
		return;

	this->setToolTip("Import DICOM data");

	mViewHeaderAction = this->createAction(this,
										   QIcon(":/icons/open_icon_library/eye.png.png"),
										   "View info", "View the header info for the first selected series",
										   SLOT(onViewHeader()));

	mImportIntoCustusXAction = this->createAction(this,
												  QIcon(":/icons/open_icon_library/arrow-right-3.png"),
												  "Import selected", "Import the selected DICOM series into the application as a volume",
												  SLOT(onImportIntoCustusXAction()));

	mDetailsAction = this->createAction(this,
										QIcon(":/icons/open_icon_library/system-run-5.png"),
										"Advanced", "Toggle additional actions",
										SLOT(toggleDetailsSlot()));

	mBrowser = new DICOMAppWidget;
	mBrowser->addActionToToolbar(mViewHeaderAction);
	mBrowser->addActionToToolbar(mImportIntoCustusXAction);
	mBrowser->addActionToToolbar(mDetailsAction);
	this->showOrHideDetails();

	mVerticalLayout->setMargin(0);
	mVerticalLayout->addWidget(mBrowser);

	this->setupDatabaseDirectory();
}

void DicomWidget::toggleDetailsSlot()
{
	bool newShowAdvancedValue = !settings()->value(mDicomShowAdvancedSettingsString, "true").toBool();
	settings()->setValue(mDicomShowAdvancedSettingsString, newShowAdvancedValue);
	this->showOrHideDetails();
}

void DicomWidget::showOrHideDetails()
{
	bool showAdvanced = settings()->value(mDicomShowAdvancedSettingsString).toBool();

	mViewHeaderAction->setVisible(showAdvanced);
	foreach (QAction* action, mBrowser->getAdvancedActions())
	{
		action->setVisible(showAdvanced);
	}
}


QString DicomWidget::getDICOMDatabaseDirectory()
{
    QString databaseDirectory = profile()->getSettingsPath() + "/DICOMDatabase";

    return databaseDirectory;
}

void DicomWidget::setupDatabaseDirectory()
{
    QString databaseDirectory = this->getDICOMDatabaseDirectory();

	QDir qdir(databaseDirectory);
	if ( !qdir.exists(databaseDirectory) )
	{
		if ( !qdir.mkpath(databaseDirectory) )
		{
			CX_LOG_CHANNEL_ERROR("dicom") << "Could not create database directory \"" << databaseDirectory;
		}
	}

    CX_LOG_CHANNEL_INFO("dicom") << "DatabaseDirectory set to: " << databaseDirectory;
	mBrowser->setDatabaseDirectory(databaseDirectory);
}

QStringList DicomWidget::currentSeriesSelection()
{
	return mBrowser->getSelectedSeries();
}

void DicomWidget::onViewHeader()
{
	QStringList series = this->currentSeriesSelection();
	std::cout << series.join("\n").toStdString() << std::endl;

	QStringList files;
	for (int i=0; i<series.size(); ++i)
	{
		QStringList current = this->getDatabase()->filesForSeries(series[i]);
		files.append(current);
	}
	files.sort();

	ctkDICOMObjectListWidget* window = new ctkDICOMObjectListWidget;
	window->setWindowTitle("DICOM File Header");
	window->setFileList(files);

	QWidget* screen = qApp->desktop()->screen(qApp->desktop()->screenNumber(this));
	QRect rect = screen->geometry();
	rect.setWidth(rect.width()*0.66);
	window->setGeometry(rect);

	window->show();
}

void DicomWidget::onImportIntoCustusXAction()
{
	QStringList series = this->currentSeriesSelection();

	if (series.empty())
        CX_LOG_CHANNEL_WARNING("dicom") << "No DICOM series selected, import failed.";

	for (unsigned i=0; i<series.size(); ++i)
	{
		this->importSeries(series[i]);
    }
}

void DicomWidget::deleteDICOMDB()
{
   CX_LOG_CHANNEL_INFO("dicom") << "Deleting DICOM database: " << this->getDICOMDatabaseDirectory();
   bool autoDeleteDICOMDB = settings()->value("Automation/autoDeleteDICOMDatabase").toBool();
   if(autoDeleteDICOMDB)
   {
	   ctkDICOMDatabase* database = this->getDatabase();
	   if(database)
	   {
		   QStringList patients = database->patients();
		   foreach(QString patient , patients)
		   {
			   this->getDatabase()->removePatient(patient);
		   }
	   }
   }
}

void DicomWidget::importSeries(QString seriesUid)
{
	cx::DicomConverter converter;
	converter.setDicomDatabase(this->getDatabase());
	cx::ImagePtr convertedImage = converter.convertToImage(seriesUid);

	if (!convertedImage)
	{
		reportError(QString("Failed to convert DICOM series %1").arg(seriesUid));
		return;
	}

	this->loadIntoPatientModel(convertedImage, seriesUid);
}

void DicomWidget::loadIntoPatientModel(ImagePtr image, QString seriesUid)
{
	VisServicesPtr services = VisServices::create(mContext);

	if (!services->patient()->isNull())
	{
		services->patient()->insertData(image);
		services->view()->autoShowData(image);
		report(QString("Loaded DICOM series %1 as %2").arg(seriesUid).arg(image->getName()));
	}
	else
	{
		reportWarning(QString("Failed to load DICOM series %1 as %2: no PatientModelService.").arg(seriesUid).arg(image->getName()));
	}
}

ctkDICOMDatabase* DicomWidget::getDatabase() const
{
	if(mBrowser)
		return mBrowser->database();
	else
		return NULL;
}

} /* namespace cx */
