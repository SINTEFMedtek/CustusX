#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"

//class SingleLayout
//{
//public:
//	ssc::View* mView;
//	ssc::SliceProxyPtr mSlicer;         
//	ssc::SliceRepSWPtr mSliceRep;  ///en vector av slice overlay....
//};

/**Widget that displays two buttons: accept and reject.
 * The result will emit a finished(bool) signal. - true=accepted, false=rejected
 * 
 * Intended use: During automated tests, the user can accept/reject the graphical
 * test result using these buttons. Connect the signal to the return value of the
 * test function or similar.
 * 
 * Clicking the button will also send a quit() to the application.
 *  
 * In automatic mode, the accept button is clicked automatically after 2 seconds.
 */
class AcceptanceBox : public QFrame
{
	Q_OBJECT
	
public:
	AcceptanceBox(QString text, QWidget* parent = 0);
	bool accepted() const;
signals:
	void finished(bool);
private:
	bool mAccepted;
	void finish(bool accepted);
	QLabel* mText;
	QPushButton* mAcceptButton;
	QPushButton* mRejectButton;
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
	void accept();
	void reject();	
};


/**Test class  with convenience methods for defining views.
 * Uses the following reps:
 *  - ssc::ToolRep3D
 *  - ssc::SliceRepSW
 *  - ssc::VolumetricRep
 */
class ViewsWindow : public QMainWindow
{
	Q_OBJECT

public:
	ViewsWindow(QString displayText, bool showSliders);
	virtual ~ViewsWindow();
	
	void define3D(const std::string& imageFilename, int r, int c);
	void defineSlice(const std::string& uid, const std::string& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	bool accepted() const { return mAcceptanceBox->accepted(); }
	
	bool mDumpSpeedData;
	
private:
	void start(bool showSliders);
	ssc::View* generateSlice(const std::string& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	ssc::ImagePtr loadImage(const std::string& imageFilename);
	void insertView(ssc::View* view, const std::string& uid, const std::string& volume, int r, int c);
	
	QSlider* mBrightnessSlider;
	QSlider *mContrastSlider;
	
	typedef std::set<ssc::View*> LayoutMap;
	std::set<ssc::View*> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;
	
	int mRenderCount;
	int mTotalRender;
	int mTotalOther;
	QTime mLastRenderEnd;
	AcceptanceBox* mAcceptanceBox; 
	
	
public slots:
	void updateRender();
	void contrast(int val);
	void brightness(int val);
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
