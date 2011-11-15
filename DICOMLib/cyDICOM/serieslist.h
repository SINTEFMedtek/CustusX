#ifndef SERIESLIST_H
#define SERIESLIST_H

#include <QtGui>

#include "ui_serieslist.h"

class SeriesList : public QDialog, private Ui_SeriesList
{
	Q_OBJECT
	public:
	SeriesList( QWidget *parent = 0 );
	void setModel( QStandardItemModel *modelParam );
	int row();
	int column();

	protected:
	void setupActions();

	private:
	QStandardItemModel *model;
	void errmsg( const char *msg );
};

#endif
