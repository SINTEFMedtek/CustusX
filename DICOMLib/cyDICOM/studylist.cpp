#include <QApplication>
#include <QtGui>

#include "studylist.h"

void StudyList::errmsg( const char *msg )
{
	QMessageBox::critical(this, tr("Oops..."), msg, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
}

void StudyList::setModel( QStandardItemModel *modelParam )
{
	model = modelParam;
	tableView->setModel( model );
	tableView->resizeColumnsToContents();
	tableView->setSelectionBehavior( QAbstractItemView::SelectRows );
}

StudyList::StudyList( QWidget *parent )
	: QDialog( parent )
{
	setupActions();
	setupUi( this );
	model = NULL;
}

int StudyList::row()
{
	return tableView->selectionModel()->currentIndex().row();
}

int StudyList::column()
{
	return tableView->selectionModel()->currentIndex().column();
}

void StudyList::setupActions()
{
//	connect( actionQuit, SIGNAL( triggered( bool ) ), qApp, SLOT( quit() ) );
//	connect( actionOpen, SIGNAL( triggered( bool ) ), this, SLOT( loadStudy() ) );
}
