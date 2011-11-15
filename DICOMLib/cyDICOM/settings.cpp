#include <QApplication>
#include <QtGui>

#include "settings.h"

void SettingsPACS::errmsg( const char *msg )
{
	QMessageBox::critical(this, tr("Oops..."), msg, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
}

SettingsPACS::SettingsPACS( QWidget *parent )
	: QDialog( parent )
{
	setupActions();
	setupUi( this );
}

void SettingsPACS::setupActions()
{
//	connect( actionQuit, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );
//	connect( actionOpen, SIGNAL( triggered( bool ) ), this, SLOT( loadStudy() ) );
}
