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

namespace cx
{

DicomWidget::DicomWidget(QWidget* parent) :
    BaseWidget(parent, "DicomWidget", "Dicom"),
	mVerticalLayout(new QVBoxLayout(this)),
	mBrowser(NULL)
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

	mBrowser = new ctkDICOMBrowser;

	mVerticalLayout->addWidget(mBrowser);
	mVerticalLayout->addLayout(buttonsLayout);

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

	mBrowser->setDatabaseDirectory(databaseDirectory);
}

void DicomWidget::onViewHeader()
{
	QStringList series = mBrowser->dicomTableManager()->currentSeriesSelection();
	std::cout << series.join("\n").toStdString() << std::endl;

	QStringList files;
	for (int i=0; i<series.size(); ++i)
	{
		QStringList current = mBrowser->database()->filesForSeries(series[i]);
		files.append(current);
	}
	files.sort();
	std::cout << "files:" << std::endl;
	std::cout << files.join("\n").toStdString() << std::endl;

	ctkDICOMObjectListWidget* window = new ctkDICOMObjectListWidget;
    window->setWindowTitle("DICOM File Header");
    window->setFileList(files);
    window->show();
}

void DicomWidget::onImportIntoCustusXAction()
{
	QStringList series = mBrowser->dicomTableManager()->currentSeriesSelection();
	std::cout << series.join("\n").toStdString() << std::endl;

	for (unsigned i=0; i<series.size(); ++i)
	{
		this->importSeries(series[i]);
	}
}

void DicomWidget::importSeries(QString seriesUid)
{
//	QStringList files = mBrowser->database()->filesForSeries(seriesUid);

	cx::DicomConverter converter;
	converter.setDicomDatabase(mBrowser->database());
	cx::ImagePtr convertedImage = converter.convertToImage(seriesUid);

	if (!convertedImage)
	{
		reportError(QString("Failed to load DICOM series %1").arg(seriesUid));
		return;
	}

//	dataManager()->loadData(convertedImage);
	report(QString("Loaded DICOM series %1 as %2").arg(seriesUid).arg(convertedImage->getName()));

//	std::cout << "import files from " << seriesUid  << ": " << std::endl;
//	std::cout << "  " << files.join("\n  ").toStdString() << std::endl;
}

} /* namespace cx */
