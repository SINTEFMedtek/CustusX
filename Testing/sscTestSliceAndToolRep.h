#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_
#include <QtGui>
#include <map>
#include "sscView.h"

#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"

class SingleLayout
{
public:
	ssc::View* mView;
	ssc::SliceProxyPtr mSlicer;
	ssc::SliceRepSWPtr mSliceRep;
};


class TestSliceAndToolRep : public QMainWindow
{
	Q_OBJECT

public:
	TestSliceAndToolRep();
	virtual ~TestSliceAndToolRep();
private:
	QWidget* mWidget;
	void start();

	ssc::View* view(const std::string& uid);
	void generateSlice(const std::string& uid, ssc::ToolPtr tool, ssc::ImagePtr image, ssc::PLANE_TYPE plane);
	void generateView(const std::string& uid);

	typedef std::map<std::string, SingleLayout> LayoutMap;
	LayoutMap mLayouts;

private slots:
	void updateRender();
};



#endif /*SSCTESTSLICEANDTOOLREP_H_*/
