#ifndef SSCEXSLICEANDTOOLREP_H_
#define SSCEXSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"

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
	ViewsWindow();
	virtual ~ViewsWindow();
	
	void define3D(const std::string& imageFilename, int r, int c);
	void defineSlice(const std::string& uid, const std::string& imageFilename, ssc::PLANE_TYPE plane, int r, int c);
	
	void insertView(ssc::View* view, const std::string& uid, const std::string& volume, int r, int c);
private:
	void start();
	ssc::View* generateSlice(const std::string& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	ssc::ImagePtr loadImage(const std::string& imageFilename);
	
	typedef std::set<ssc::View*> LayoutMap;
	std::set<ssc::View*> mLayouts;
	QGridLayout* mSliceLayout;
	
public slots:
	void updateRender();
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
