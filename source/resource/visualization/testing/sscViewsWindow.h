#ifndef SSCTESTSLICEANDTOOLREP_H_
#define SSCTESTSLICEANDTOOLREP_H_

#include <QMainWindow>
#include "sscView.h"
class QGridLayout;

namespace cxtest
{

/** MainWindow for testing of Views and Reps.
 */
class ViewsWindow : public QMainWindow
{
	Q_OBJECT

public:
	ViewsWindow();
	virtual ~ViewsWindow();

	void setDescription(const QString& desc);

	// setup views
	cx::ViewWidget* addView(QString caption, int r, int c);
	cx::ViewWidget* add2DView(QString caption, int r, int c);
	bool quickRunWidget();

	cx::View* getView(int index);

private:
	void prettyZoom(cx::View *view);
	void insertView(cx::ViewWidget *view, const QString& uid, const QString& volume, int r, int c);

	std::vector<cx::View *> mLayouts;
	QGridLayout* mSliceLayout;
	QString mDisplayText;

	double mZoomFactor;
	QTimer* mRenderingTimer;

public slots:
	void updateRender();
};

} // namespace cxtest

#endif /*SSCTESTSLICEANDTOOLREP_H_*/
