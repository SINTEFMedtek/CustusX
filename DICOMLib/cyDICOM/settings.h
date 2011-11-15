#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGui>

#include "ui_settingsPACS.h"

class SettingsPACS : public QDialog,
	public Ui_settingsPACS
{
	Q_OBJECT
	public:
	SettingsPACS( QWidget *parent = 0 );

	protected:
	void setupActions();

	private:
	void errmsg( const char *msg );
};

#endif
