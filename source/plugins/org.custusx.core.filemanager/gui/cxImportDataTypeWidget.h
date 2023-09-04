/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMPORTDATATYPEWIDGET_H
#define CXIMPORTDATATYPEWIDGET_H

#include "org_custusx_core_filemanager_Export.h"
#include <QPushButton>
#include <QTableWidgetItem>
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxLogger.h"
#include "cxVisServices.h"
#include "cxFileManagerService.h"
#include "cxPatientModelService.h"
#include "cxRegistrationTransform.h"
#include "cxDataInterface.h"
class QTableWidget;
class QStackedWidget;

namespace cx
{

class ImportWidget;

class org_custusx_core_filemanager_EXPORT ImportDataTypeWidget : public BaseWidget
{
	Q_OBJECT
public:
	ImportDataTypeWidget(ImportWidget *parent, VisServicesPtr services, std::vector<DataPtr> data, std::vector<DataPtr> &parentCandidates, QString filename, IMAGE_MODALITY modalitySuggestion = imUNKNOWN, IMAGE_SUBTYPE subtype = istUNKNOWN);
	~ImportDataTypeWidget();

	static QSize getQTableWidgetSize(QTableWidget *t);
	static int findRowIndexContainingButton(QPushButton *button, QTableWidget *tableWidget);

	//Functions used by SimpleImportDataDialog
	QTableWidget* getSimpleTableWidget();
	std::vector<DataPtr> getDatas() {return mData;};
	void setData(std::vector<DataPtr> datas) {mData = datas;}

	const int mCheckBoxCTColumn;
	const int mCheckBoxPETColumn;
	const int mCheckBoxPETCTColumn;

public slots:
	void update();
	void prepareDataForImport();

private slots:
	virtual void showEvent(QShowEvent *event);
	void pointMetricGroupSpaceChanged(int index);
	void updateImageType();
	void tableItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void removeRowFromTableAndDataFromImportList();

protected:
	void setModality(ImagePtr image, IMAGE_MODALITY modalitySuggestion, IMAGE_SUBTYPE subtype = istUNKNOWN);
	QTableWidgetItem *createCheckbox(QString text);

private:
	void createDataSpecificGui(int index, IMAGE_MODALITY modalitySuggestion, IMAGE_SUBTYPE subtype);
	void updateTableWithNumberOfSlices(ImagePtr image);
	void updateTableWithSliceSpacing(ImagePtr image);
	std::map<QString, QString> getParentCandidateList();

	void updateSpaceComboBox(QComboBox *box, QString space);
	void updateParentCandidatesComboBox();

	void importAllData();

	void applyParentTransformImport();
	void applyConversionLPS();
	void applyConversionToUnsigned();

	//Use heuristics to guess a parent frame, based on similarities in name.
	QString getInitialGuessForParentFrame();
	int similatiryMeasure(QString current, QString candidate);
	QStringList splitStringIntoSeparateParts(QString current);
	int countEqualListElements(QStringList first, QStringList second);
	bool excludeElement(QString element);
	QString removeParenthesis(QString current);

	void addPointMetricGroupsToTable();

	bool isInputFileInNiftiFormat();
	bool isSegmentation(QString filename);

	ImportWidget* mImportWidget;
	VisServicesPtr mServices;
	std::vector<DataPtr> mData;
	QString mFilename;
	std::vector<DataPtr> &mParentCandidates;

	std::map<QString, QComboBox *> mSpaceCBs;
	std::map<QString, std::vector<DataPtr> > mPointMetricGroups;

	QComboBox *mAnatomicalCoordinateSystems;
	QComboBox *mShouldImportParentTransform;
	QComboBox *mParentCandidatesCB;
	QCheckBox *mShouldConvertDataToUnsigned;

	QTableWidget* mTableWidget;
	QStringList mTableHeader;
	int mSelectedIndexInTable;

	//image specific
	QStackedWidget *mStackedWidgetImageParameters;
	StringPropertyDataModalityPtr mModalityAdapter;
	StringPropertyImageTypePtr mImageTypeAdapter;
	QWidget* mImageTypeCombo;
	QWidget* mModalityCombo;

	int mSeriesNumColumn = 1;
	int mNumSlicesColumn = 2;
	int mFilenameColumn = 3;
	int mTypeColumn = 4;
	int mSliceSpacingColumn = 5;
	int mSpaceColumn = 6;

	QTableWidgetItem *mCheckBoxWidgetCT;
	QTableWidgetItem *mCheckBoxWidgetPET;
	QTableWidgetItem *mCheckBoxWidgetPETCT;
};

}
#endif // CXIMPORTDATATYPEWIDGET_H
