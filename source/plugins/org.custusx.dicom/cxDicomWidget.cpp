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

#include "cxDicomWidget.h"
#include "ctkDICOMBrowser.h"
#include "ctkDICOMAppWidget.h"
#include "cxDICOMAppWidget.h"
#include "cxDataLocations.h"
#include <QDir>
#include "ctkDICOMTableManager.h"
#include "ctkDICOMObjectListWidget.h"
#include "cxTypeConversions.h"
#include "cxDicomConverter.h"
#include "cxReporter.h"
//#include "cxLegacySingletons.h"
//#include "cxDataManager.h"
#include "cxLogger.h"

#include "cxPatientModelService.h"

#include "ctkPluginContext.h"
#include "ctkServiceTracker.h"
#include <QApplication>
#include <QDesktopWidget>
#include "cxDicomImageReader.h"

namespace cx
{

DicomWidget::DicomWidget(ctkPluginContext *context, QWidget *parent) :
	BaseWidget(parent, "DicomWidget", "Dicom"),
	mVerticalLayout(new QVBoxLayout(this)),
	mBrowser(NULL),
	mContext(context)
{
	this->setModified();
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

	QHBoxLayout* buttonsLayout = new QHBoxLayout;

	//Add detailed button
	mViewHeaderAction = this->createAction(this,
										   QIcon(),
										   "View Header", "View header info for first selected series",
										   SLOT(onViewHeader()),
										   buttonsLayout);
	mImportIntoCustusXAction = this->createAction(this,
												  QIcon(),
												  "Import", "Import selected series into application",
												  SLOT(onImportIntoCustusXAction()),
												  buttonsLayout);

	mBrowser = new DICOMAppWidget;

	mVerticalLayout->addWidget(mBrowser);
	mVerticalLayout->addLayout(buttonsLayout);


//	connect(mBrowser->dicomTableManager(),
//	        SIGNAL(seriesSelectionChanged(const QItemSelection&, const QItemSelection&)),
//	        this,
//	        SLOT(onModelSelected(const QItemSelection&,const QItemSelection&)));


	this->setupDatabaseDirectory();
}


DicomWidget::~DicomWidget()
{
}

QString DicomWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Dicom plugin.</h3>"
			"<p>Import data from dicom</p>"
			"<p>The dicom widgets are taken from the ctk project.</p>"
			"</html>";
}

void DicomWidget::setupDatabaseDirectory()
{
	QString databaseDirectory = DataLocations::getSettingsPath() + "/DICOMDatabase";

	QDir qdir(databaseDirectory);
	if ( !qdir.exists(databaseDirectory) )
	{
		if ( !qdir.mkpath(databaseDirectory) )
		{
			std::cerr << "Could not create database directory \"" << databaseDirectory.toLatin1().data() << "\".\n";
		}
	}

	std::cout << "databaseDirectory: " << databaseDirectory << std::endl;
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
	std::cout << "files:" << std::endl;
	std::cout << files.join("\n").toStdString() << std::endl;

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

	for (unsigned i=0; i<series.size(); ++i)
	{
		this->importSeries(series[i]);
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
	ctkServiceTracker<PatientModelService*> tracker(mContext);
	tracker.open();
	PatientModelService* service = tracker.getService(); // get arbitrary instance of this type

	if (service)
	{
		service->insertData(image);
		report(QString("Loaded DICOM series %1 as %2").arg(seriesUid).arg(image->getName()));
	}
	else
	{
		reportWarning(QString("Failed to load DICOM series %1 as %2: no PatientModelService.").arg(seriesUid).arg(image->getName()));
	}
}

//void DicomWidget::onModelSelected(const QItemSelection &item1, const QItemSelection &item2)
//{
//	Q_UNUSED(item1);
//	Q_UNUSED(item2);

//	QStringList series = this->currentSeriesSelection();

//	for (int i=0; i<series.size(); ++i)
//	{
//		this->printFrameCountForSeries(series[i]);
//	}

////	std::cout << "selected: " << series.join("\n") << std::endl;
//}

void DicomWidget::printFrameCountForSeries(QString series) const
{
	QString seriesDescription;
	int frameCount = 0;
	QStringList files = this->getDatabase()->filesForSeries(series);
	for (unsigned i=0; i<files.size(); ++i)
	{
		DicomImageReaderPtr reader = DicomImageReader::createFromFile(files[i]);
		if (!reader)
			continue;
		frameCount += reader->getNumberOfFrames();
		seriesDescription = reader->item()->GetElementAsString(DCM_SeriesDescription);
	}
	std::cout << QString("%1 frames for series %2").arg(frameCount).arg(seriesDescription) << std::endl;
}

ctkDICOMDatabase* DicomWidget::getDatabase() const
{
	return mBrowser->database();
}

} /* namespace cx */
