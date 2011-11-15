#ifndef SEARCHPACS_H
#define SEARCHPACS_H

#include <QtGui>

#include "DICOMLib.h"
#include "ui_searchPACS.h"

class SearchPACS : public QDialog, private Ui_searchPACS
{
	Q_OBJECT
	public:
	SearchPACS( QWidget *parent = 0 );
	const char *searchString();
	enum pacs_search_param_t searchParam();
	const char *searchTime();

	protected:
	void setupActions();

	private:
	void errmsg( const char *msg );
};

#endif
