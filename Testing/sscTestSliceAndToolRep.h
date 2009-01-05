#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_
#include <QtGui>
#include "sscView.h"
class TestSliceAndToolRep : public QMainWindow
{
	Q_OBJECT
	
public:
	TestSliceAndToolRep();
	virtual ~TestSliceAndToolRep();
private:
	QWidget* mWidget; 
	void start();
	ssc::View* viewA;
	ssc::View* viewC;
	ssc::View* viewS;
	ssc::View* view3D ;
private slots:
	void updateRender();
	
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
