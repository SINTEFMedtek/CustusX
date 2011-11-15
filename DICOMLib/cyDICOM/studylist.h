#ifndef STUDYLIST_H
#define STUDYLIST_H

#include <QtGui>

#include "ui_studylist.h"

class StudyList : public QDialog, private Ui_StudyList
{
	Q_OBJECT
	public:
	StudyList( QWidget *parent = 0 );
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
