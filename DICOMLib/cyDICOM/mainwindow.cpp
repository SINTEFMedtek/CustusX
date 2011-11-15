#define GL_GLEXT_PROTOTYPES 1

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <QApplication>
#include <QtGui>

#include <assert.h>

#include "studylist.h"
#include "serieslist.h"
#include "searchPACS.h"
#include "settings.h"
#include "mainwindow.h"

#include "dcmtk/dcmdata/dcuid.h"

void MainWindow::errmsg( const char *msg )
{
	QMessageBox::critical(this, tr("Oops..."), msg, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
}

MainWindow::MainWindow( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi( this );
	setupActions();
	studyList = NULL;
	currentStudy = NULL;
	seriesList = NULL;
	currentSeries = NULL;
	volume = NULL;
	internalFormat = GL_LUMINANCE;
	pixelFormat = GL_LUMINANCE;
	pixelType = GL_UNSIGNED_BYTE;
	DICOMLib_Init();
	DICOMLib_Network( "SONOWAND", "ANY-SCP", "SONOWAND", "10.200.31.117", 5104 );
}

MainWindow::~MainWindow()
{
	DICOMLib_Done();
}

void MainWindow::setupActions()
{
	connect( actionQuit, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );
	connect( actionOpen, SIGNAL( triggered( bool ) ), this, SLOT( loadStudy() ) );
	connect( actionOpen_DICOMDIR, SIGNAL( triggered( bool ) ), this, SLOT( loadDICOMDIR() ) );
	connect( actionOpen_PACS, SIGNAL( triggered( bool ) ), this, SLOT( loadPACS() ) );
	connect( action_Export, SIGNAL( triggered( bool ) ), this, SLOT( exportDICOM() ) );
	connect( actionSettingsPACS, SIGNAL( triggered( bool ) ), this, SLOT( settingsPACS() ) );
	connect( actionAuto, SIGNAL( triggered( bool ) ), this, SLOT( autoVOI() ) );
	connect( actionNone, SIGNAL( triggered( bool ) ), this, SLOT( noVOI() ) );
	connect( WindowCenter, SIGNAL( valueChanged( int ) ), this, SLOT( setCenter( int ) ) );
	connect( WindowWidth, SIGNAL( valueChanged( int ) ), this, SLOT( setWidth( int ) ) );
	connect( WindowReject, SIGNAL( valueChanged( int ) ), this, SLOT( setReject( int ) ) );
}

void MainWindow::setReject( int r )
{
	WindowReject->setSliderPosition( r );
	WindowReject->setToolTip( QString().setNum( WindowReject->value() ) );
	view->setThreshold( (float)r / 100.0 );
}

void MainWindow::setWidth( int w )
{
	WindowWidth->setSliderPosition( w );
	WindowWidth->setToolTip( QString().setNum( WindowWidth->value() ) );
	view->setContrast( (float)w / 100.0 );
}

void MainWindow::setCenter( int c )
{
	WindowCenter->setSliderPosition( c );
	WindowCenter->setToolTip( QString().setNum( WindowCenter->value() ) );
	view->setBrightness( (float)c * 2.0 / 100.0 );
}

static int progress( int value )
{
	if ( value == -1 ) return 0;
	printf( "Progress: %d\n", value );
//	usleep( 500 );
	return 0;
}

void MainWindow::processStudy( struct study_t *study )
{
	struct study_t *iterStudy;
	int i, studies_count = 0, selected;

	// Count number of studies
	for (iterStudy = study; iterStudy; studies_count++, iterStudy = iterStudy->next_study );

	/// --- Select study ---

	QStandardItemModel model( studies_count, 5 );
	// Add headers
	model.setHeaderData( 0, Qt::Horizontal, QString( "Patient Name" ) );
	model.setHeaderData( 1, Qt::Horizontal, QString( "Patient ID" ) );
	model.setHeaderData( 2, Qt::Horizontal, QString( "Study ID" ) );
	model.setHeaderData( 3, Qt::Horizontal, QString( "Series" ) );
	model.setHeaderData( 4, Qt::Horizontal, QString( "Birth date" ) );
	for ( i = 0, iterStudy = study; iterStudy; i++, iterStudy = iterStudy->next_study )
	{
		model.setData( model.index( i, 0, QModelIndex()), QString( iterStudy->patientName ) );
		model.setData( model.index( i, 1, QModelIndex()), QString( iterStudy->patientID ) );
		model.setData( model.index( i, 2, QModelIndex()), QString( iterStudy->studyID ) );
		model.setData( model.index( i, 3, QModelIndex()), QString::number( iterStudy->series_count ) );
		model.setData( model.index( i, 4, QModelIndex()), QString( iterStudy->patientBirthDate ) );
	}
	StudyList list;
	list.setModel( &model );
	list.exec();

	selected = list.row();
	if ( selected >= studies_count || selected < 0 )
	{
		errmsg( "No study selected" );
		return;
	}
	printf( "Selected study %d\n", selected );
	for ( i = 0; study && i < selected; study = study->next_study, i++ ) printf( "Skipped a study i=%d\n", i );
	if ( !study )
	{
		errmsg( "Ooops... Ran out of studies in list?!" );
		return;
	}
	currentStudy = study;
}

void MainWindow::processSeries( struct series_t *series, int series_count )
{
	int i, selected;

	/// --- Select series ---
	assert( series );

	QStandardItemModel model2( series_count, 7 );
	// Add headers
	model2.setHeaderData( 0, Qt::Horizontal, QString( "Series ID" ) );
	model2.setHeaderData( 1, Qt::Horizontal, QString( "Modality" ) );
	model2.setHeaderData( 2, Qt::Horizontal, QString( "Series Date" ) );
	model2.setHeaderData( 3, Qt::Horizontal, QString( "Frames" ) );
	model2.setHeaderData( 4, Qt::Horizontal, QString( "Valid" ) );
	model2.setHeaderData( 5, Qt::Horizontal, QString( "SOP Class" ) );
	model2.setHeaderData( 6, Qt::Horizontal, QString( "Info" ) );
	for ( i = 0, series = seriesList; series; i++, series = series->next_series )
	{
		model2.setData( model2.index( i, 0, QModelIndex()), QString( series->seriesID ) );
		model2.setData( model2.index( i, 1, QModelIndex()), QString( series->modality ) );
		model2.setData( model2.index( i, 2, QModelIndex()), QString( series->seriesDate ) );
		model2.setData( model2.index( i, 3, QModelIndex()), QString::number( series->frames ) );
		model2.setData( model2.index( i, 4, QModelIndex()), series->valid ? QString( "Yes" ) : QString( "No" ) );
		if ( series->series_info[strlen(series->series_info)-1] == '\n' ) series->series_info[strlen(series->series_info)-1] = '\0';
		model2.setData( model2.index( i, 5, QModelIndex()), QString( dcmFindNameOfUID( series->SOPClassUID ) ) );
		model2.setData( model2.index( i, 6, QModelIndex()), QString( series->series_info ) );
	}
	SeriesList list2;
	list2.setModel( &model2 );
	list2.exec();

	selected = list2.row();
	if ( selected >= series_count || selected < 0 )
	{
		errmsg( "No series selected" );
		return;
	}
	printf( "Selected series %d\n", selected );
	for ( i = 0, series = seriesList; series && i < selected; series = series->next_series, i++ ) printf( "Skipped series %s\n", series->seriesID );;

	/// etc

	statusBar()->showMessage( tr( "Study successfully loaded" ), 5000 );
	if ( volume )
	{
		free( (void *)volume );
		volume = NULL;
		glDeleteTextures( 1, texnames );
	}

	DICOMLib_UseAutoVOI( series );
	printf( "Called AutoVOI\n" );
	volume = DICOMLib_GetVolume( series, progress );
	progress( -1 );
	if ( !volume )
	{
		printf( "Could not load volume: %s\n", series->series_info );
		errmsg( "Nothing found to display!" );
		return;
	}
	currentSeries = series;

	sizes[0] = volume->x;
	sizes[1] = volume->y;
	sizes[2] = volume->z;

	// Set window setting sliders
/*
	WindowCenter->setMaximum( (int)(series->VOI.minmax.center + series->VOI.minmax.width / 2) );
	WindowCenter->setMinimum( (int)(series->VOI.minmax.center - series->VOI.minmax.width / 2) );
	WindowWidth->setMaximum( (int)series->VOI.minmax.width );
	WindowWidth->setMinimum( 1 );
	WindowCenter->setSliderPosition( (int)series->VOI.current.center );
	WindowWidth->setSliderPosition( (int)series->VOI.current.width );
*/
	WindowCenter->setMaximum( 100 );
	WindowWidth->setMaximum( 100 );
	WindowWidth->setMinimum( 1 );
	WindowWidth->setSliderPosition( 0 );
	WindowCenter->setMinimum( 1 );
	WindowCenter->setSliderPosition( 50 );
	WindowReject->setSliderPosition( 7 );
	WindowReject->setMaximum( 100 );
	WindowReject->setMinimum( 0 );

	WindowCenter->setToolTip( QString().setNum( WindowCenter->value() ) );
	WindowWidth->setToolTip( QString().setNum( WindowWidth->value() ) );
	WindowReject->setToolTip( QString().setNum( WindowReject->value() ) );

	// Set OpenGL types
	switch ( series->samples_per_pixel )
	{
	case 1: pixelFormat = internalFormat = GL_LUMINANCE; break;
	case 2: pixelFormat = internalFormat = GL_LUMINANCE_ALPHA; break;
	case 3: pixelFormat = GL_RGB; internalFormat = GL_RGBA; break;
	case 4: pixelFormat = internalFormat = GL_RGBA; break;
	}
/*	switch ( series->bits_per_sample )
	{
	case 8: pixelType = GL_UNSIGNED_BYTE; break;
	case 16: pixelType = GL_UNSIGNED_SHORT; break;
	case 32: pixelType = GL_UNSIGNED_INT; break;
	}
	printf( "samples_per_pixel=%d bits_per_sample=%d\n", series->samples_per_pixel, series->bits_per_sample ); fflush(NULL);
*/
	// Upload to GPU
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
	glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
	glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_TEXTURE_3D );
	glClear( GL_COLOR_BUFFER_BIT );

	glGenTextures( 1, texnames );
	glBindTexture( GL_TEXTURE_3D, texnames[0] );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexImage3D( GL_TEXTURE_3D, 0, internalFormat, sizes[0], sizes[1], sizes[2], 0, pixelFormat, pixelType, volume->volume );
	view->enable();
}

void MainWindow::loadStudy()
{
	QString directory = QFileDialog::getExistingDirectory( this, tr( "Please pick the folder containing the study" ), QString( "/Data" ) );
	struct study_t *study = DICOMLib_StudiesFromPath( directory.toAscii(), progress );

	progress( -1 );
	if ( !study )
	{
		errmsg( "Could not load study" );
		return;
	}
	processStudy( study );
	seriesList = DICOMLib_GetSeries( currentStudy, NULL );
	if ( seriesList )
	{
		processSeries( seriesList, currentStudy->series_count );
	}
	else
	{
		errmsg( "No series found!" );
	}
}

void MainWindow::loadDICOMDIR()
{
	QString directory = QFileDialog::getOpenFileName( this, tr( "Please pick the DICOMDIR file containing the study" ), QString( "/Data" ), tr("DICOMDIR") );
	struct study_t *study = DICOMLib_StudiesFromDICOMDIR( directory.toAscii() );

	progress( -1 );
	if ( !study )
	{
		errmsg( "Could not load DICOMDIR" );
		return;
	}
	processStudy( study );
	seriesList = DICOMLib_GetSeries( currentStudy, NULL );
	if ( seriesList )
	{
		processSeries( seriesList, currentStudy->series_count );
	}
	else
	{
		errmsg( "No series found!" );
	}
}

void MainWindow::loadPACS()
{
	SearchPACS pacs;
	struct study_t *study;

	pacs.exec();
	study = DICOMLib_StudiesFromPACS( pacs.searchString(), pacs.searchParam(), pacs.searchTime() );
	if ( !study )
	{
		errmsg( "Could not load from PACS" );
		return;
	}
	processStudy( study );
	printf( "Expected series: %d\n", currentStudy->series_count );
	DICOMLib_FetchStudy( currentStudy->studyInstanceUID );	// request the study from the PACS server
}

void MainWindow::exportDICOM()
{
	if ( currentSeries && currentStudy )
	{
		QString directory = QFileDialog::getSaveFileName( this, tr( "Please pick a filename to export the series to" ) );
		struct series_t *tmpS = currentStudy->first_series;
		struct volume_t *tmpV = currentSeries->volume;

		currentStudy->first_series = currentSeries;	// hack
		currentSeries->volume = (volume_t *)volume;	// hack
		if ( DICOMLib_WriteSeries( directory.toAscii(), currentStudy, false, DICOMLIB_PROFILE_USB, DICOMLIB_ALIGNMENT_TABLE ) != 0 )
		{
			errmsg( "Could not export study" );
		}
		currentStudy->first_series = tmpS;		// hack it back
		currentSeries->volume = tmpV;			// hack it back
	}
}

void MainWindow::settingsPACS()
{
	SettingsPACS pacs;

	pacs.exec();
	
	DICOMLib_Network( pacs.appAET->text().toAscii(), pacs.peerAET->text().toAscii(), pacs.destAET->text().toAscii(), pacs.hostName->text().toAscii(), pacs.hostName->text().toInt() );
}

void MainWindow::autoVOI()
{
	if ( currentSeries )
	{
		if ( volume )
		{
			free( (void *)volume );
			volume = NULL;
		}
		DICOMLib_UseAutoVOI( currentSeries );
		volume = DICOMLib_GetVolume( currentSeries, progress );
		glTexImage3D( GL_TEXTURE_3D, 0, internalFormat, sizes[0], sizes[1], sizes[2], 0, pixelFormat, pixelType, volume->volume );
	}
}

void MainWindow::noVOI()
{
	if ( currentSeries )
	{
		if ( volume )
		{
			free( (void *)volume );
			volume = NULL;
		}
		DICOMLib_NoVOI( currentSeries );
		volume = DICOMLib_GetVolume( currentSeries, progress );
		glTexImage3D( GL_TEXTURE_3D, 0, internalFormat, sizes[0], sizes[1], sizes[2], 0, pixelFormat, pixelType, volume->volume );
	}
}
