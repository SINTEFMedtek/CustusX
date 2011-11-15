// Yeah, should have combined StudyList and SeriesList classes into a generic list class.
// But... Bother. - Per

#include <QApplication>
#include <QtGui>

#include "serieslist.h"

void SeriesList::errmsg( const char *msg )
{
	QMessageBox::critical(this, tr("Oops..."), msg, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
}

void SeriesList::setModel( QStandardItemModel *modelParam )
{
	model = modelParam;
	tableView->setModel( model );
	tableView->resizeColumnsToContents();
	tableView->setSelectionBehavior( QAbstractItemView::SelectRows );
}

SeriesList::SeriesList( QWidget *parent )
	: QDialog( parent )
{
	setupActions();
	setupUi( this );
	model = NULL;
}

int SeriesList::row()
{
	return tableView->selectionModel()->currentIndex().row();
}

int SeriesList::column()
{
	return tableView->selectionModel()->currentIndex().column();
}

void SeriesList::setupActions()
{
//	connect( actionQuit, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );
//	connect( actionOpen, SIGNAL( triggered( bool ) ), this, SLOT( loadSeries() ) );
}
