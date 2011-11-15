#include <QApplication>
#include <QtGui>

#include "searchPACS.h"

void SearchPACS::errmsg( const char *msg )
{
	QMessageBox::critical(this, tr("Oops..."), msg, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
}

SearchPACS::SearchPACS( QWidget *parent )
	: QDialog( parent )
{
	setupActions();
	setupUi( this );
}

const char *SearchPACS::searchString()
{
	return lineEdit->text().toAscii();
}

enum pacs_search_param_t SearchPACS::searchParam()
{
	if ( patientName->isChecked() )
	{
		return DICOMLIB_SEARCH_PATIENT_NAME;
	}
	else if ( patientID->isChecked() )
	{
		return DICOMLIB_SEARCH_PATIENT_ID;
	}
	else if ( patientBirthDate->isChecked() )
	{
		return DICOMLIB_SEARCH_PATIENT_BIRTH;
	}
	else if ( studyDate->isChecked() )
	{
		return DICOMLIB_SEARCH_STUDY_DATE;
	}
	else if ( accessionNumber->isChecked() )
	{
		return DICOMLIB_SEARCH_ACCESSION_NUMBER;
	}

	return DICOMLIB_SEARCH_NONE;
}

const char *SearchPACS::searchTime()
{
#if 0
    QRadioButton *olderThanDay;
    QRadioButton *olderThanMonth;
    QRadioButton *olderThanWeek;
    QRadioButton *olderThanEver;
#endif
	return NULL;
}

void SearchPACS::setupActions()
{
//	connect( actionQuit, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );
//	connect( actionOpen, SIGNAL( triggered( bool ) ), this, SLOT( loadStudy() ) );
}
