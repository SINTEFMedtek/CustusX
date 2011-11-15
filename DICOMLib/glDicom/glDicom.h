#ifndef GLDICOM_H
#define GLDICOM_H

#include <QApplication>

#include "ui_gldicom.h"

class testMain : public QMainWindow, public Ui_MainWindow
{
	Q_OBJECT

public:
	testMain(QWidget *parent = 0);
	~testMain();

protected:
	void setupActions();
};

#endif // GLDICOM_H
