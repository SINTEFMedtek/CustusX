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

namespace cx
{

DicomWidget::DicomWidget(QWidget* parent) :
    BaseWidget(parent, "DicomWidget", "Dicom"),
    mVerticalLayout(new QVBoxLayout(this))
{

/*
	  // set up Qt resource files
	  QResource::registerResource("./Resources/ctkDICOM.qrc");

	  QSettings settings;
	  QString databaseDirectory;

	  // set up the database
	  if (argc > 1)
	  {
	    QString directory(argv[1]);
	    settings.setValue("DatabaseDirectory", directory);
	    settings.sync();
	  }

	  if ( settings.value("DatabaseDirectory", "") == "" )
	  {
	    databaseDirectory = QString("./ctkDICOM-Database");
	    std::cerr << "No DatabaseDirectory on command line or in settings.  Using \"" << databaseDirectory.toLatin1().data() << "\".\n";
	  } else
	  {
	    databaseDirectory = settings.value("DatabaseDirectory", "").toString();
	  }
*/

	QString databaseDirectory = DataLocations::getSettingsPath() + "/DICOMDatabase";

	QDir qdir(databaseDirectory);
	if ( !qdir.exists(databaseDirectory) )
	{
		if ( !qdir.mkpath(databaseDirectory) )
		{
			std::cerr << "Could not create database directory \"" << databaseDirectory.toLatin1().data() << "\".\n";
		}
	}


	mBrowser = new ctkDICOMBrowser;
	mBrowser->setDatabaseDirectory(databaseDirectory);

	mVerticalLayout->addWidget(mBrowser);
//	  ctkDICOMBrowser DICOMApp;
//
//	  DICOMApp.setDatabaseDirectory(databaseDirectory);
//	  DICOMApp.show();


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

} /* namespace cx */
