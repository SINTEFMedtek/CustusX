#ifndef CXIMPORTDATATYPEWIDGET_H
#define CXIMPORTDATATYPEWIDGET_H

#include "org_custusx_core_filemanager_Export.h"
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxLogger.h"
#include  <QPushButton>
#include "cxVisServices.h"
#include "cxFileManagerService.h"
#include "cxPatientModelService.h"
#include "cxRegistrationTransform.h"
#include "cxDataInterface.h"

class QTableWidget;

namespace cx
{

class org_custusx_core_filemanager_EXPORT ImportDataTypeWidget : public BaseWidget
{
	Q_OBJECT
public:
	ImportDataTypeWidget(QWidget *parent, VisServicesPtr services, std::vector<DataPtr> data, std::vector<DataPtr> &parentCandidates);

private slots:
	virtual void showEvent(QShowEvent *event);
	void pointMetricGroupSpaceChanged(int index);

private:
	QStringList getParentCandidateList();

	QWidget *mPointMetricWidget;

	/** Use heuristics to guess a parent frame,
	 *  based on similarities in name.
	 */
	//void setInitialGuessForParentFrame(DataPtr data);

	VisServicesPtr mServices;
	std::vector<DataPtr> mData;
	std::vector<DataPtr> &mParentCandidates;
	std::map<QString, std::vector<DataPtr> > mPointMetricGroups;

	QComboBox *mAnatomicalCoordinateSystems;
	QComboBox *mShouldImportParentTransform;
	QComboBox *mParentCandidatesCB;

	QTableWidget* mTableWidget;
	QStringList mTableHeader;
	int mSelectedIndexInTable;

};

}
#endif // CXIMPORTDATATYPEWIDGET_H