#ifndef SSCTESTSLICEANDOVERLAYIMAGE_H_
#define SSCTESTSLICEANDOVERLAYIMAGE_H_
#include <QtGui>

#include <map>
#include <vtkImageData.h>
#include "sscView.h"
#include "sscImage.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscBlendedSliceRep.h"
#include "sscImageBlenderProxy.h"
#include "sscSlicedImageProxy.h"
#include "sscSurfaceRep.h" 

class SingleLayout
{
public:
	SingleLayout();
	virtual ~SingleLayout();
	ssc::View* mView;	
	ssc::SliceProxyPtr mSlicer;
	ssc::SliceRepSWPtr mSliceRep;
	ssc::BlendedSliceRepPtr mBlendedSliceRep;
	ssc::VolumetricRepPtr mVolumetricRep;
	ssc::SliceProxyPtr mToolProxy;
	ssc::SurfaceRepPtr mSurfaceRep; 
	std::string getUid()const;
			
private:
	std::string mUid;

};

typedef boost::shared_ptr<class SingleLayout> SingleLayoutPtr;

class TestSliceAndOverlayImage : public QMainWindow
{
	Q_OBJECT
public:
	TestSliceAndOverlayImage();
	virtual ~TestSliceAndOverlayImage();
	void generateLayerdSliceRep(ssc::ToolPtr tool, ssc::ImageBlenderProxyPtr blender, ssc::PLANE_TYPE plane);
	void generateSliceRepLayerd(ssc::ToolPtr tool, std::vector<ssc::ImagePtr> images, ssc::PLANE_TYPE plane);
	void generateVolumeInVolumeRep(ssc::ImageBlenderProxyPtr blender,ssc::PLANE_TYPE plane);
	void generateSurfaceRep(ssc::ImagePtr images,ssc::PLANE_TYPE plane);
	void generatePlanarVolumeRep(ssc::ImagePtr images,ssc::PLANE_TYPE plane);
	
private:
	QWidget* mWidget;
	QRadioButton* radio1;
	ssc::ImageBlenderProxyPtr mBlenderProxy;
	
	void start();
	ssc::View* view(ssc::PLANE_TYPE plane);
	void generateView(ssc::PLANE_TYPE plane);	
	void addImageToView(const std::string& imageUid);
	
	typedef std::map< ssc::PLANE_TYPE, SingleLayoutPtr> LayoutMap;
	LayoutMap mLayouts;
signals:
	void changeColor();
	
private slots:
	void updateRender();
	void updateAlpha();
};

class MyColorDialog : public QDialog
{
	Q_OBJECT
public:
	MyColorDialog(QWidget* parent=0);
	~MyColorDialog();
	QColor getColor(){ return mColor; };
signals:
	void colorChanged(QColor);
	
private slots:
	void setColor();
	
private:
	QColor mColor;
	QString str;
	QPushButton* button;
	QLabel* colorLabel;
	QVBoxLayout* layout;
};

class OverlayControlWidget : public QWidget
{
	Q_OBJECT
public:
	OverlayControlWidget(QWidget* parent=0, Qt::WindowFlags f=0 );
	~OverlayControlWidget();
	void setImage(ssc::ImagePtr images);
	
private:
	void setupWidget();
	void setupAction();
	void createFirstCLUT();
	QSlider *mContrastSlider;	
	QSlider *mBrightness;
	QSlider *mAlphaSlider;
	
	QSlider *m3DContrast;	
	QSlider *m3DBrightness;
	QSlider *m3DAlphaSlider;
	
	QPushButton* buttonRemove;
	QPushButton* changeColorButton;
	MyColorDialog* myColorDialog;
	ssc::ImagePtr mImage;
	vtkLookupTablePtr mCLut; 
	std::string mImageUid;
	QSpinBox* mTresholdVal;
	
private slots:

	void alphaSlider(int val);
	void brightness(int val);
	void contrast(int val);
	void changeColorSlot(QColor color);
	void Contrast3D(int val);
	void Brightness3D(int val);
	void llrSlot(int val);
	void setThreshold( int val);
	
};

#endif





