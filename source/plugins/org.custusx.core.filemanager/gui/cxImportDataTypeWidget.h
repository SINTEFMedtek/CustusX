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

class ImportWidget;

class org_custusx_core_filemanager_EXPORT ImportDataTypeWidget : public BaseWidget
{
	Q_OBJECT
public:
	ImportDataTypeWidget(ImportWidget *parent, VisServicesPtr services, std::vector<DataPtr> data, std::vector<DataPtr> &parentCandidates, QString filename);
	~ImportDataTypeWidget();

public slots:
	void update();
	void prepareDataForImport();

private slots:
	virtual void showEvent(QShowEvent *event);
	void pointMetricGroupSpaceChanged(int index);
	void updateImageType();

private:
	void createDataSpecificGui(DataPtr data);
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
	StringPropertyDataModalityPtr mModalityAdapter;
	StringPropertyImageTypePtr mImageTypeAdapter;
	QWidget* mImageTypeCombo;
	QWidget* mModalityCombo;
};

}
#endif // CXIMPORTDATATYPEWIDGET_H
