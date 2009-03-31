#include "sscTestSliceAndOverlayImage.h"
#include <iostream>
#include <string>
#include <vector>

#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkLookupTable.h>

#include "sscTestUtilities.h"
#include "sscDataManager.h"
#include "sscAxesRep.h"
#include "sscSliceRep.h"

#include "sscVolumetricRep.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscDummyToolManager.h"
#include "sscDummyTool.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscSlicePlaneRep.h"
#include "sscImageBlenderProxy.h"
#include "sscSlicedImageProxy.h"

using ssc::Vector3D;
using ssc::Transform3D;
using ssc::IntBoundingBox3D;
using ssc::DoubleBoundingBox3D;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	TestSliceAndOverlayImage test;
	int val = app.exec();
	return val;
}


MyColorDialog ::MyColorDialog (QWidget* parent)
{
		 
	button = new QPushButton("Select color");
	connect(button, SIGNAL(clicked()), this, SLOT(setColor()) );
	
	colorLabel = new QLabel;
	layout = new QVBoxLayout;
	layout->addWidget(button);
	layout->addWidget(colorLabel);
	setLayout(layout);
}

MyColorDialog::~MyColorDialog()
{
	
}

void MyColorDialog::setColor()
{
	QColor color = QColorDialog::getColor(Qt::green, this);
	if (color.isValid())
	{
		colorLabel->setText(color.name());
	    colorLabel->setPalette(QPalette(color));
	    colorLabel->setAutoFillBackground(true);
	    
	    std::cout<<" Button pressed"<<std::endl;
	    
	 }	
	emit colorChanged(color);
}

//----------------------------
//buttons and slider for overlay 
//----------------------------
OverlayControlWidget::OverlayControlWidget(QWidget* parent ,Qt::WindowFlags f ):
	QWidget(parent,f)	
{
	this->setupWidget();		
	this->setupAction();
}

OverlayControlWidget::~OverlayControlWidget()
{
}

void OverlayControlWidget::setupAction()
{
	//slider action 
	connect(mContrastSlider, SIGNAL(sliderMoved(int)), this, SLOT(contrast(int)) );	
	connect(mBrightness, SIGNAL(sliderMoved(int)), this, SLOT(brightness(int)) );
	connect(mAlphaSlider, SIGNAL(sliderMoved(int)), this, SLOT(alphaSlider(int)) );
	connect(myColorDialog, SIGNAL(colorChanged(QColor)), SLOT(changeColorSlot(QColor)) );  
	
	//3d slider
	connect(m3DContrast, SIGNAL(sliderMoved(int)), this, SLOT(Contrast3D(int)) );	
	connect(m3DBrightness, SIGNAL(sliderMoved(int)), this, SLOT(Brightness3D(int)) );
	connect(m3DAlphaSlider, SIGNAL(sliderMoved(int)), this, SLOT(llrSlot(int)) ); //LLR

}

void OverlayControlWidget::Contrast3D(int val)
{
	mImage->getTransferFunctions3D().setLevel(val/1.0);
}

void OverlayControlWidget::Brightness3D(int val)
{
	mImage->getTransferFunctions3D().setLevel(val/1.0);
}

void OverlayControlWidget::llrSlot(int val)
{
	mImage->getLookupTable2D().setLLR(val/10.0);
}
void OverlayControlWidget::changeColorSlot(QColor color)	
{
	mImage->getLookupTable2D().addNewColor(color); 
}
void OverlayControlWidget::alphaSlider(int val)
{
	mImage->setAlpha( val/10.0 );
	
}
void OverlayControlWidget::brightness(int val)
{
	mImage->getLookupTable2D().setLevel(val/1.0);
}

void OverlayControlWidget::contrast(int val)
{
	mImage->getLookupTable2D().setWindow(val/1.0);	
}

void OverlayControlWidget::setThreshold( int val)
{
	mImage->getLookupTable2D().setAlphaRange(val/10.0);
}

void OverlayControlWidget::setupWidget()
{
	
	QVBoxLayout* overAllLayout = new QVBoxLayout;
	QHBoxLayout* topSliderLayout = new QHBoxLayout;	
	QVBoxLayout* slidersLayout = new QVBoxLayout;
	
	mBrightness = new QSlider(Qt::Horizontal);	
	slidersLayout->addWidget(new QLabel(tr("Brightness")) );
	slidersLayout ->addWidget(mBrightness);
	mContrastSlider = new QSlider(Qt::Horizontal);
	slidersLayout->addWidget(new QLabel(tr("Contrasst")) );
	slidersLayout ->addWidget(mContrastSlider);
	topSliderLayout ->addLayout(slidersLayout);
	
	mAlphaSlider= new QSlider(Qt::Vertical);
	topSliderLayout ->addWidget(new QLabel(tr("Alpha")) );
	topSliderLayout ->addWidget(mAlphaSlider);
	

	//mTresholdVal = new QSpinBox;  
	//slidersLayout ->addWidget(mTresholdVal); 	

	QVBoxLayout* slidersLayout_3D = new QVBoxLayout;
	m3DContrast = new QSlider(Qt::Horizontal);
	slidersLayout_3D->addWidget(new QLabel(tr("3D - contrast")) );
	slidersLayout_3D->addWidget(m3DContrast);
	m3DBrightness = new QSlider(Qt::Horizontal);
	slidersLayout_3D->addWidget(new QLabel(tr("3D - Brightness")) );
	slidersLayout_3D->addWidget(m3DBrightness);
	m3DAlphaSlider = new QSlider(Qt::Horizontal);
	slidersLayout_3D->addWidget(new QLabel(tr("3D - Opacity")) );
	slidersLayout_3D->addWidget(m3DAlphaSlider);
	

	topSliderLayout->addLayout(slidersLayout_3D );
	
	overAllLayout->addLayout(topSliderLayout);
	
	
	QHBoxLayout* topButtonLayout = new QHBoxLayout;
	QVBoxLayout* buttonsLayout = new QVBoxLayout;
	buttonRemove = new QPushButton("Remove Image");
	changeColorButton = new QPushButton("Change Color");
	buttonsLayout->addWidget(changeColorButton );
	buttonsLayout->addWidget(buttonRemove);
	topButtonLayout->addLayout(buttonsLayout);
	
	QVBoxLayout* colorLayout = new QVBoxLayout;
	myColorDialog = new MyColorDialog;
	colorLayout->addWidget(myColorDialog);
	topButtonLayout->addLayout(colorLayout);
	overAllLayout->addLayout(topButtonLayout);
	this->setLayout(overAllLayout);
	
}

void OverlayControlWidget::setImage(ssc::ImagePtr image)
{
	mImage = image;
	mImageUid = image->getUid();

	int window = 10 * (int)mImage->getLookupTable2D().getWindow();
	int level = 10 * (int)mImage->getLookupTable2D().getLevel();
	double alpha = mImage->getAlpha();

	std::cout<<"window: "<<window<<std::endl;
	std::cout<<"level: "<<level<<std::endl;
	std::cout<<"alpha: "<<alpha<<std::endl;
	std::cout<<"--------"<<std::endl;
	
	//update slidebar...
	mContrastSlider->setMaximum(256);
	mContrastSlider->setValue(256);
	
	mBrightness->setMaximum(256);
	mBrightness->setValue(128);
	
	mAlphaSlider->setMaximum(10);
	mAlphaSlider->setValue((int)alpha * 10);
	
	m3DContrast->setMaximum(256);
	m3DContrast->setValue(256);
	
	m3DBrightness->setMaximum(256);
	m3DBrightness->setValue(128);
	
	
	m3DAlphaSlider->setMaximum(10);
	m3DAlphaSlider->setValue(10);
}

void OverlayControlWidget::createFirstCLUT()
{
}

//----------------------------
//end button  widget class
//----------------------------


SingleLayout::SingleLayout()
{
	//mUid = uid;
}

SingleLayout::~SingleLayout()
{
}

std::string SingleLayout::getUid()const 
{
	return mUid;
}

TestSliceAndOverlayImage::TestSliceAndOverlayImage()
{
	
	this->resize(QSize(1200, 1000));
	this->show();
	mWidget = new QWidget;
	this->setCentralWidget( mWidget );
	start();
}

TestSliceAndOverlayImage::~TestSliceAndOverlayImage()
{}

//this creats the amount of views we will use in a layout... 
void TestSliceAndOverlayImage::generateView(ssc::PLANE_TYPE plane)
{
	SingleLayoutPtr single;
	single.reset(new SingleLayout());
	
	//create the reps..
	ssc::SliceRepSWPtr sliceRep = ssc::SliceRepSW::New("SliceRepSW");
	ssc::VolumetricRepPtr volumeRep = ssc::VolumetricRep::New("VolumetricRep");
	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());
	ssc::BlendedSliceRepPtr layeredRep = ssc::BlendedSliceRep::New("LayerSliceRep");
	
	single->mView =  new ssc::View();
	single->mSliceRep = sliceRep;
	single->mToolProxy = proxy;
	single->mVolumetricRep = volumeRep;
	single->mBlendedSliceRep = layeredRep;
	
	mLayouts.insert( std::make_pair(plane, single) );
}	

void TestSliceAndOverlayImage::addImageToView(const std::string& imageUid)
{
	std::cout<< "addImageToView,  uid: "<< imageUid <<std::endl;
	for (LayoutMap::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		if(iter->first == ssc::ptNOPLANE)
			continue;
		std::cout<< "add plane"<<iter->first<<std::endl;
		iter->second->mView->getRenderer()->SetBackground(0.5,0.5,0.5);
		iter->second->mView->addRep( iter->second->mSliceRep);	
	}	
}

//Test Blend first then slice - update slicer 
void TestSliceAndOverlayImage::generateLayerdSliceRep(ssc::ToolPtr tool, ssc::ImageBlenderProxyPtr blender, ssc::PLANE_TYPE plane)
{
	std::cout<<"\ngenerateLayerdSliceRep "<<std::endl;	
	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());
	proxy->setTool(tool);
	proxy->setFollowType(ssc::ftFIXED_CENTER);
	proxy->setOrientation(ssc::otORTHOGONAL);
	proxy->setPlane(plane);

	mLayouts[plane]->mSliceRep->setSliceProxy(proxy);
	mLayouts[plane]->mSliceRep->setInput( blender->getOutput() ); //3D dataset
	mLayouts[plane]->mSliceRep->update();
	mLayouts[plane]->mView->addRep(mLayouts[plane]->mSliceRep); 
}

//test Slice first then Blend.. update hole pipe..
void TestSliceAndOverlayImage::generateSliceRepLayerd(ssc::ToolPtr tool, std::vector<ssc::ImagePtr> images, ssc::PLANE_TYPE plane)
{
	std::cout<<"\ngenerateSliceRepLayerd,"<< images.size()<<" image to blend "<<std::endl;
	ssc::SliceProxyPtr proxy(new ssc::SliceProxy());

	proxy->setTool(tool);
	proxy->setFollowType(ssc::ftFIXED_CENTER);
	proxy->setOrientation(ssc::otORTHOGONAL);
	proxy->setPlane(plane);
	
	mLayouts[plane]->mBlendedSliceRep->setSliceProxy(proxy);
	mLayouts[plane]->mBlendedSliceRep->setImages(images);
	
	mLayouts[plane]->mView->addRep(mLayouts[plane]->mBlendedSliceRep);
	 
}

// volume rep ..NOT FINISHED
void TestSliceAndOverlayImage::generateVolumeInVolumeRep(ssc::ImageBlenderProxyPtr blender,ssc::PLANE_TYPE plane)
{
	
//	ssc::VolumetricRepPtr mRepPtr = mLayouts[plane]->mVolumetricRep;
//	mRepPtr->setInput(blender->getOutput());
//	mRepPtr->setName("Blending mode");	
//	mLayouts[plane]->mView->addRep(mRepPtr);	
}

//Test wiht iso-surface--->
void TestSliceAndOverlayImage::generateSurfaceRep(ssc::ImagePtr images,ssc::PLANE_TYPE plane)
{
	ssc::SurfaceRepPtr surfaceRep = ssc::SurfaceRep::New("ssc::SurfaceRep");  
	surfaceRep->setImage(images);
	//mLayouts[plane]->mSurfaceRep = surfaceRep;
	mLayouts[plane]->mView->addRep(surfaceRep);
}

// TODO: TEST WITH PLANAR VOLUME
void TestSliceAndOverlayImage::generatePlanarVolumeRep(ssc::ImagePtr images,ssc::PLANE_TYPE plane)
{
	
}

ssc::View* TestSliceAndOverlayImage::view(ssc::PLANE_TYPE uid)
{
	return mLayouts[uid]->mView;
}
	
void TestSliceAndOverlayImage::start()
{
	// generate imageFileName
	std::string mImageFileName1 = ssc::TestUtilities::ExpandDataFileName("Fantomer/Kaisa/MetaImage/Kaisa.mhd");
	std::string	mImageFileName2 = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T105136_MRT1.mhd");
	std::string mImageFileName3  = ssc::TestUtilities::ExpandDataFileName("MetaImage/20070309T102309_MRA.mhd");
	std::cout << mImageFileName1 << std::endl;
	std::cout << mImageFileName2  << std::endl;
	std::cout << mImageFileName3  << std::endl;

	// read image
	ssc::ImagePtr image1 = ssc::DataManager::getInstance()->loadImage(mImageFileName1 , ssc::rtMETAIMAGE);
	ssc::ImagePtr image2 = ssc::DataManager::getInstance()->loadImage(mImageFileName2 , ssc::rtMETAIMAGE);
	ssc::ImagePtr image3 = ssc::DataManager::getInstance()->loadImage(mImageFileName2 , ssc::rtMETAIMAGE);
	
	//store imags	
	std::vector<ssc::ImagePtr> images;
	images.push_back(image1);
	images.push_back(image2);
	//images.push_back(image3);	
			

	// Initialize dummy toolmanager.
	ssc::ToolManager* mToolmanager = ssc::DummyToolManager::getInstance();
	mToolmanager->configure();
	mToolmanager->initialize();
	mToolmanager->startTracking();

	ssc::ToolPtr tool = mToolmanager->getDominantTool();
	connect( tool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), this, SLOT( updateRender()));
//
	
	//generateViews layout
	generateView(ssc::ptAXIAL);
	generateView(ssc::ptCORONAL);
	generateView(ssc::ptSAGITTAL);
	generateView(ssc::ptNOPLANE); //3D view
	
	//we are going with this one on 2d overlay
	generateSliceRepLayerd(tool, images, ssc::ptAXIAL);
	generateSliceRepLayerd(tool, images, ssc::ptCORONAL);
	generateSliceRepLayerd(tool, images, ssc::ptSAGITTAL);
		
	
	/*--- 3D overLay.. using vtkImageBlend... not good! ---*/
	//generateVolumeInVolumeRep(mBlenderProxy, ssc::ptNOPLANE);
	
	/** 3D Surface, normal iso-surface using contour filter...**/
	//generateSurfaceRep( images[1] ,ssc::ptNOPLANE);
	
	
	/** 3D Planar rendering, normal, must test update position **/
	//generatePlanarVolumeRep(images, ssc::ptNOPLANE);
	
	//Tool 3D rep
	//ssc::ToolRep3DPtr toolRep = ssc::ToolRep3D::New( tool->getUid(), tool->getName() );
	//toolRep->setTool(tool);
	//view("3D")->addRep(toolRep);
	
	//-Layout stuff- 
	QHBoxLayout* mainLayout = new QHBoxLayout;
	//slicerView lauout
	QGridLayout* sliceLayout = new QGridLayout;
	sliceLayout->addWidget( view(ssc::ptCORONAL), 0, 0);
	sliceLayout->addWidget( view(ssc::ptSAGITTAL), 0, 1);
	sliceLayout->addWidget( view(ssc::ptAXIAL), 1, 0);
	sliceLayout->addWidget( view(ssc::ptNOPLANE), 1, 1);
	mainLayout->addLayout(sliceLayout);//Slice layout	
	
	//button layout
	QVBoxLayout* buttonLayout = new QVBoxLayout;
	
	for(unsigned int i=0; i< images.size(); ++i)
	{
		OverlayControlWidget *blendingControll_1 = new OverlayControlWidget;
		blendingControll_1->setImage(images[i]);
		radio1 = new QRadioButton(tr("This active volume"));
		buttonLayout->addWidget(radio1);
		buttonLayout->addWidget(blendingControll_1);
	}
	
	mainLayout->addLayout(buttonLayout); //Buttons
	mWidget->setLayout(mainLayout);
		
	for (LayoutMap::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		iter->second->mView->getRenderer()->ResetCamera();
	}
	updateRender();
}

//SLOTS USED ..
void TestSliceAndOverlayImage::updateAlpha()
{
	if(mBlenderProxy)
	{
		mBlenderProxy->updateAlpha();	
	}
	
}

void TestSliceAndOverlayImage::updateRender()
{
	for (LayoutMap::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		iter->second->mView->getRenderWindow()->Render();
	}
}
