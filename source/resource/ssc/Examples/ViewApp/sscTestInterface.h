#ifndef SSCTESTINTERFACE_H_
#define SSCTESTINTERFACE_H_

#include <QMainWindow>

#include "sscImage.h"
#include "sscVolumetricRep.h"
/**
 * sscTestInterface.h
 *
 * \brief
 *
 * \date Nov 3, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
class QMenu;
class QMenuBar;
class QAction;

class vtkObject;
class vtkCallbackCommand;

namespace ssc
{
class View;
class ToolManager;

class TestInterface : public QMainWindow
{
	Q_OBJECT

public:
	TestInterface();
	~TestInterface();

public slots:
	void testDummyToolAndDummyToolManager();
	void testSwitchingViewsRepsAndImages();

private slots:
	void setImage(int i);
	void setRep(int i);
	void setView(int i);

private:
	static void timerUpdate(vtkObject* obj, unsigned long, void*, void*);
	void startRendering();

	QMenu* mTestMenu;

	QAction* mTestDummyToolAndDummyToolManagerAction;
	QAction* mTestSwitchingViewsRepsAndImagesAction;

	ToolManager* mToolmanager;
	vtkCallbackCommand* mCallback;
	ViewWidget *mView1;
	ViewWidget *mView2;
	ImagePtr mImagePtr1;
	ImagePtr mImagePtr2;
	VolumetricRepPtr mRepPtr1;
	VolumetricRepPtr mRepPtr2;

	ImagePtr mCurrentImage;
	VolumetricRepPtr mCurrentRep;
	ViewWidget *mCurrentView;
};
}//namespace ssc

#endif /* SSCTESTINTERFACE_H_ */
