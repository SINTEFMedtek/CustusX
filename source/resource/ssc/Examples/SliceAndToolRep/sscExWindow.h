#ifndef SSCEXSLICEANDTOOLREP_H_
#define SSCEXSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscVolumetricRep.h"
//#include "sscDICOMWidget.h"
//#include "sscGPURayCastVolumetricRep.h"


/**Test class  with convenience methods for defining views.
 * Uses the following reps:
 *  - ssc::ToolRep3D
 *  - ssc::SliceRepSW
 *  - ssc::VolumetricRep
 */
class ViewsWindow : public QWidget
{
	Q_OBJECT

public:
	ViewsWindow();
	virtual ~ViewsWindow();
	
//	void define3D(const QString& imageFilename, int r, int c);
	void defineGPU_3D(const QString& imageFilename, int r, int c);
	void defineSlice(const QString& uid, const QString& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	
	void define3D(const QString& imageFilename, int r, int c);
	void define3D(ssc::ImagePtr image, int r, int c);

	void insertView(ssc::View* view, const QString& uid, const QString& volume, int r, int c);
private:
	ssc::View* generateSlice(const QString& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	ssc::ImagePtr loadImage(const QString& imageFilename);
	
	typedef std::set<ssc::View*> LayoutMap;
	std::set<ssc::View*> mLayouts;
	QGridLayout* mSliceLayout;
	QTimer* mRenderingTimer;
	QLineEdit* mSpeedEdit;
//	ssc::DICOMWidget* mDICOMWidget;
 // ssc::GPURayCastVolumetricRepPtr mVolumetricRep;

public slots:
	void updateRender();
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
