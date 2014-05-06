/*
 * cxDicomWidget.cpp
 *
 *  Created on: May 2, 2014
 *      Author: christiana
 */

#include "cxDicomWidget.h"
#include "ctkDICOMBrowser.h"
#include "cxDataLocations.h"
#include <QDir>
#include "ctkDICOMTableManager.h"
#include "ctkDICOMObjectListWidget.h"
#include "cxTypeConversions.h"

namespace cx
{

DicomWidget::DicomWidget(QWidget* parent) :
    BaseWidget(parent, "DicomWidget", "Dicom"),
    mVerticalLayout(new QVBoxLayout(this))
{
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
//	mBrowser->setDatabaseDirectory(databaseDirectory);

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
	QStringList files = mBrowser->database()->filesForSeries(seriesUid);

	std::cout << "import files from " << seriesUid  << ": " << std::endl;
	std::cout << "  " << files.join("\n  ").toStdString() << std::endl;
}

} /* namespace cx */
