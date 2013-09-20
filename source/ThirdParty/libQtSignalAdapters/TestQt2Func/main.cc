#include <iostream>

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>

#include <QApplication>
#include <QMetaObject>
#include <QMetaMethod>
#include <QWidget>

#include <QtSignalAdapters/Qt2Func.h>
#include <QtSignalAdapters/ConnectionFactories.h>

#include "ui_SimpleGUI.h"

using namespace std;
using namespace QtSignalAdapters;

void func0()
{
	printf("func0\n");
}

void func1(int i)
{
	printf("[func1] Invoked with i = %d\n", i);
}

void func2(int i1, int i2)
{
}

void func3(int i1, int i2, int i3)
{
}


class Class1
{
public:
	Class1() {}

	void method1(int i)
	{
		printf("[Class1::method1] Invoked with i = %d\n", i);
	}
};

class FuncClass
{
public:
	void operator()(int i)
	{
		printf("[FuncClass::operator()] Invoked with i = %d\n", i);
	}
};

void connectEm(Ui_Form* uif)
{
	connect0<void()>(uif->pushButton,
		SIGNAL(clicked()), boost::bind(func0));
}

int
main(int argc, char** argv)
{
	QApplication app(argc, argv);

	Ui_Form* uif = new Ui_Form;
	QWidget* widget = new QWidget;

	uif->setupUi(widget);

	widget->show();

	Class1 class1;
	FuncClass funcClass;


	QtConn* vcConn1;
	QtConn* vcConn2;
	QtConn* vcConn3;

	QtConn* vcConn4;

	QtConn* vcConn5;

	connectEm(uif);

	vcConn1 = connect1<void(int)>(uif->spinBox,
		SIGNAL(valueChanged(int)), boost::bind(func1, _1));
	vcConn2 = connect1<void(int)>(uif->spinBox,
		SIGNAL(valueChanged(int)), boost::bind(&Class1::method1, &class1, _1));
	vcConn3 = connect1<void(int)>(uif->spinBox,
		SIGNAL(valueChanged(int)), boost::bind<void>(funcClass, _1));

	vcConn4 = connect2<void(int,int)>(uif->spinBox,
		SIGNAL(valueChanged(int,int)), boost::bind<void>(func2, _1, _2));

	vcConn5 = connect3<void(int,int,int)>(uif->spinBox,
		SIGNAL(valueChanged(int,int,int)), boost::bind<void>(func3, _1, _2, _3));

	app.exec();
}
