#ifndef CXOPERATINGTABLEWIDGET_H
#define CXOPERATINGTABLEWIDGET_H

#include "cxPreferenceTab.h"
#include "cxCoreServices.h"
#include "cxVector3D.h"

namespace cx
{
class Transform3DWidget;
typedef boost::shared_ptr<class Vector3DProperty> Vector3DPropertyPtr;


/**
 * Widget for viewing and setting the OperatingTable from PatientModelService.
 */
class cxGui_EXPORT OperatingTableWidget : public QWidget
{
	Q_OBJECT
public:
	OperatingTableWidget(CoreServicesPtr services, QWidget* parent=NULL);
	virtual ~OperatingTableWidget() {}

	void showEvent(QShowEvent *event);

private:
	void matrixWidgetChanged();
	void backendChanged();

	QVBoxLayout* mVerticalLayout;
	CoreServicesPtr mServices;
	QLabel* mLabel;
	Transform3DWidget* mMatrixWidget;
	Vector3DPropertyPtr mTableUp;

	void onDefineDownWithTool();
	void setNewUp(Vector3D newUp);
	void onDefinePatientAnteriorUp();
	void onDefinePatientPosteriorUp();
	void onDefinePatientSuperiorUp();
};


/** Wrap TableWidget in a PreferenceTab
 */
class cxGui_EXPORT OperatingTableTab : public PreferenceTab
{
	Q_OBJECT

public:
	OperatingTableTab(CoreServicesPtr services, QWidget *parent = 0);
	void init();

public slots:
	void saveParametersSlot() {}

protected:
	CoreServicesPtr mServices;

private slots:
};

} /* namespace cx */


#endif // CXOPERATINGTABLEWIDGET_H
