/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWSWINDOW_H_
#define CXVIEWSWINDOW_H_

#include "cxtestresourcevisualization_export.h"

#include <QMainWindow>
#include "cxView.h"
#include "cxViewCollectionWidget.h"

class QGridLayout;

namespace cxtest
{

/** MainWindow for testing of Views and Reps.
 */
class CXTESTRESOURCEVISUALIZATION_EXPORT ViewsWindow : public QMainWindow
{
	Q_OBJECT

public:
	ViewsWindow(cx::RenderWindowFactoryPtr factory);
	virtual ~ViewsWindow();

	void setDescription(const QString& desc);

	// setup views
	cx::ViewPtr addView(cx::View::Type type, int r, int c);
	cx::ViewPtr add2DView(int r, int c);
	bool quickRunWidget();
	void clearLayoutWidget();

	cx::ViewPtr getView(int index);

private:
	void prettyZoom(cx::ViewPtr view);
	void setNiceSize();

	std::vector<cx::ViewPtr > mViews;
	cx::ViewCollectionWidget* mLayoutWidget;

	double mZoomFactor;
	QTimer* mRenderingTimer;

public slots:
	void updateRender();
};

} // namespace cxtest

#endif /*CXVIEWSWINDOW_H_*/
