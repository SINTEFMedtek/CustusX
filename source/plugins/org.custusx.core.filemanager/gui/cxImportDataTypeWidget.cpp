/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImportDataTypeWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QFileInfo>
#include "cxOptionsWidget.h"
#include "cxFileReaderWriterService.h"
#include "cxFileManagerService.h"
#include "cxLogger.h"
#include "cxImage.h"
#include "cxPointMetric.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxVolumeHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxViewService.h"
#include "cxImportWidget.h"
#include "cxCustomMetaImage.h"

namespace cx
{

ImportDataTypeWidget::ImportDataTypeWidget(ImportWidget *parent, VisServicesPtr services, std::vector<DataPtr> data, std::vector<DataPtr> &parentCandidates, QString filename) :
	BaseWidget(parent, "ImportDataTypeWidget", "Import"),
	mImportWidget(parent),
	mServices(services),
	mData(data),
	mFilename(filename),
	mParentCandidates(parentCandidates),
	mSelectedIndexInTable(0),
	mImageTypeCombo(NULL),
	mModalityCombo(NULL)

{
	mAnatomicalCoordinateSystems = new QComboBox();
	mAnatomicalCoordinateSystems->addItem("LPS"); //CX
	mAnatomicalCoordinateSystems->addItem("RAS"); //NIfTI

	if(isInputFileInNiftiFormat())
		mAnatomicalCoordinateSystems->setCurrentText("RAS");

	mShouldImportParentTransform = new QComboBox();
	mShouldImportParentTransform->addItem("No");
	mShouldImportParentTransform->addItem("Yes");

	mParentCandidatesCB = new QComboBox();

	mShouldConvertDataToUnsigned = new QCheckBox();
	mShouldConvertDataToUnsigned->setCheckState(Qt::Unchecked);

	mTableWidget = new QTableWidget();
	mTableWidget->setRowCount(0);
	mTableWidget->setColumnCount(4);
	mTableHeader<<"#"<<"Type"<<"Name"<<"Space";
	mTableWidget->setHorizontalHeaderLabels(mTableHeader);
	mTableWidget->horizontalHeader()->setStretchLastSection(true);
	mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	mTableWidget->verticalHeader()->setVisible(false);
	mTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	mTableWidget->setShowGrid(false);
	mTableWidget->setStyleSheet("QTableView {selection-background-color: #ACCEF7;}");
	mTableWidget->setGeometry(QApplication::desktop()->screenGeometry());

	QString type, name;
	for(unsigned i=0; i<mData.size(); ++i)
	{
		if(!mData[i])
		{
			CX_LOG_WARNING() << "ImportDataTypeWidget::ImportDataTypeWidget: No data";
			continue;
		}
		type = mData[i]->getType();
		name = mData[i]->getName();
		QString space = mData[i]->getSpace();
		//create point metric groups
		if(type == PointMetric::getTypeName())
		{
			space = boost::dynamic_pointer_cast<PointMetric>(mData[i])->getSpace().toString();
			(mPointMetricGroups[space]).push_back(mData[i]);
		}
		//add image or mesh directly to the table
		else
		{
			int newRowIndex = mTableWidget->rowCount();
			mTableWidget->setRowCount(newRowIndex+1);
			mTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem("1"));
			mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(name));
			mTableWidget->setItem(newRowIndex, 2, new QTableWidgetItem(type));
			mTableWidget->setItem(newRowIndex, 3, new QTableWidgetItem(space));
		}
		this->createDataSpecificGui(mData[i]);
	}
	this->addPointMetricGroupsToTable();

	//gui
	QVBoxLayout *topLayout = new QVBoxLayout(this);
	this->setLayout(topLayout);

	QFileInfo fileInfo(filename);
	QString title = fileInfo.fileName();

	QGroupBox *groupBox = new QGroupBox(title);

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget(new QLabel("For all data in the file: "), 0, 0, 1, 2);
	gridLayout->addWidget(new QLabel("Specify anatomical coordinate system"), 1, 0);
	gridLayout->addWidget(mAnatomicalCoordinateSystems, 1, 1);
	gridLayout->addWidget(new QLabel("Import parents transform?"), 2, 0);
	gridLayout->addWidget(mShouldImportParentTransform, 2, 1);
	gridLayout->addWidget(new QLabel("Set parent"), 3, 0);
	gridLayout->addWidget(mParentCandidatesCB, 3, 1);
	gridLayout->addWidget(new QLabel("Convert data to unsigned?"), 4, 0);
	gridLayout->addWidget(mShouldConvertDataToUnsigned, 4,1);
	gridLayout->addWidget(mTableWidget, 5, 0, 1, 2);
	if(mModalityCombo)
		gridLayout->addWidget(mModalityCombo);
	if(mImageTypeCombo)
		gridLayout->addWidget(mImageTypeCombo);

	groupBox->setLayout(gridLayout);
	topLayout->addWidget(groupBox);

	connect(mImportWidget, &ImportWidget::readyToImport, this, &ImportDataTypeWidget::prepareDataForImport);
	connect(mImportWidget, &ImportWidget::parentCandidatesUpdated, this, &ImportDataTypeWidget::update);
}

ImportDataTypeWidget::~ImportDataTypeWidget()
{
	disconnect(mImportWidget, &ImportWidget::readyToImport, this, &ImportDataTypeWidget::prepareDataForImport);
	disconnect(mImportWidget, &ImportWidget::parentCandidatesUpdated, this, &ImportDataTypeWidget::update);
}


void ImportDataTypeWidget::createDataSpecificGui(DataPtr data)
{
	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);

	if(image)
	{
		mModalityAdapter = StringPropertyDataModality::New(mServices->patient());
		mModalityCombo = new LabeledComboBoxWidget(this, mModalityAdapter);
		mModalityAdapter->setData(image);

		mImageTypeAdapter = StringPropertyImageType::New(mServices->patient());
		mImageTypeCombo = new LabeledComboBoxWidget(this, mImageTypeAdapter);
		mImageTypeAdapter->setData(image);

		if(isInputFileInNiftiFormat()) // NIfTI files are usually MR. Set this as the default
		{
			mModalityAdapter->setValue(enum2string(imMR));
			updateImageType();
		}
	}
}

void ImportDataTypeWidget::updateImageType()
{
	// Test code: Trying to use convertToImageSubType on file name to find correct subtype.
	IMAGE_SUBTYPE imageSubType = convertToImageSubType(mFilename);
	mImageTypeAdapter->setValue(enum2string(imageSubType));
}

std::map<QString, QString> ImportDataTypeWidget::getParentCandidateList()
{
	std::map<QString, QString> parentCandidates;
	for(unsigned i=0; i<mParentCandidates.size(); ++i)
	{
		parentCandidates[mParentCandidates[i]->getName()] = mParentCandidates[i]->getUid();
	}

	return parentCandidates;
}

void ImportDataTypeWidget::updateSpaceComboBox(QComboBox *box, QString pointMetricGroupId)
{
	box->clear();
	std::map<QString, QString> parentCandidates = this->getParentCandidateList();
	std::map<QString, QString>::iterator it;
	for(it= parentCandidates.begin(); it != parentCandidates.end(); ++it)
	{
		QVariant id(it->second);
		box->addItem(it->first, id);
	}
	std::vector<DataPtr> pointMetricGroup = mPointMetricGroups[pointMetricGroupId];
	QString currentSpace = pointMetricGroup[0]->getSpace();
	box->setCurrentText(currentSpace);
}

void ImportDataTypeWidget::updateParentCandidatesComboBox()
{
	//remember selection
	QString selectedParentId = (mParentCandidatesCB->itemData(mParentCandidatesCB->currentIndex()).toString());

	mParentCandidatesCB->clear();
	std::map<QString, QString> parentCandidates = this->getParentCandidateList();
	std::map<QString, QString>::iterator it;
	QVariant emptyId("");
	mParentCandidatesCB->addItem("", emptyId);
	int selectedIndex = 0;
	for(it= parentCandidates.begin(); it != parentCandidates.end(); ++it)
	{
		QString idString = it->second;
		QVariant id(idString);
		mParentCandidatesCB->addItem(it->first, id);
		if(selectedParentId.compare(idString, Qt::CaseInsensitive) == 0)
			selectedIndex = mParentCandidatesCB->count()-1;
	}

	if(selectedIndex != 0)
		mParentCandidatesCB->setCurrentIndex(selectedIndex);
	else
	{
		//TODO parent guess:
		QString parentGuess = this->getInitialGuessForParentFrame();
		//CX_LOG_DEBUG() << "ParentGuess: " << parentGuess;
		mParentCandidatesCB->setCurrentText(parentGuess);
	}


}

void ImportDataTypeWidget::importAllData()
{
	for(unsigned i=0; i<mData.size(); ++i)
	{
		if(mData[i])
		{
			QString parentId = (mParentCandidatesCB->itemData(mParentCandidatesCB->currentIndex()).toString());
			mData[i]->get_rMd_History()->setParentSpace(parentId);

			mServices->patient()->insertData(mData[i]);
			mServices->view()->autoShowData(mData[i]);
		}
	}
}

void ImportDataTypeWidget::applyParentTransformImport()
{
	CX_LOG_DEBUG() << "applyParentTransformImport()";

	QString parentId = (mParentCandidatesCB->itemData(mParentCandidatesCB->currentIndex()).toString());
	DataPtr parent = mServices->patient()->getData(parentId);

	if(!parent)
	{
		CX_LOG_ERROR() << "Could not find parent data with uid: " << parentId;
		return;
	}

	std::vector<DataPtr>::iterator it = mData.begin();
	for(;it!=mData.end(); ++it)
	{
		DataPtr data = (*it);
		data->get_rMd_History()->setRegistration(parent->get_rMd());
		report("Assigned rMd from data [" + parent->getName() + "] to data [" + data->getName() + "]");
	}

	/*
	if (!mTransformFromParentFrameCheckBox->isChecked())
	return;
	if(!mData)
	return;
	DataPtr parent = mPatientModelService->getData(mData->getParentSpace());
	if (!parent)
	return;
	mData->get_rMd_History()->setRegistration(parent->get_rMd());
	report("Assigned rMd from data [" + parent->getName() + "] to data [" + mData->getName() + "]");
	*/
}

void ImportDataTypeWidget::applyConversionLPS()
{
	CX_LOG_DEBUG() << "applyConversionLPS()";

	std::vector<DataPtr>::iterator it = mData.begin();
	for(;it!=mData.end(); ++it)
	{
		DataPtr data = (*it);
		Transform3D sMd = data->get_rMd();
		Transform3D sMr = createTransformFromReferenceToExternal(pcsRAS);
		Transform3D rMd = sMr.inv() * sMd;
		data->get_rMd_History()->setRegistration(rMd);
		report("Nifti import: Converted data " + data->getName() + " from LPS to RAS coordinates.");
	}

	/*
	if (!mNiftiFormatCheckBox->isChecked())
		return;
	if(!mData)
		return;
	Transform3D sMd = mData->get_rMd();
	Transform3D sMr = createTransformFromReferenceToExternal(pcsRAS);
	//  rMd = createTransformRotateZ(M_PI) * rMd;
	Transform3D rMd = sMr.inv() * sMd;
	mData->get_rMd_History()->setRegistration(rMd);
	report("Nifti import: Converted data " + mData->getName() + " from LPS to RAS coordinates.");
	*/

}

void ImportDataTypeWidget::applyConversionToUnsigned()
{
	CX_LOG_DEBUG() << "applyConversionToUnsigned()";

	std::vector<DataPtr>::iterator it = mData.begin();
	for(;it!=mData.end(); ++it)
	{
		DataPtr data = (*it);

		ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
		if (!image)
			return;

		ImagePtr converted = convertImageToUnsigned(mServices->patient(), image);

		image->setVtkImageData(converted->getBaseVtkImageData());

		ImageTF3DPtr TF3D = converted->getTransferFunctions3D()->createCopy();
		ImageLUT2DPtr LUT2D = converted->getLookupTable2D()->createCopy();
		image->setLookupTable2D(LUT2D);
		image->setTransferFunctions3D(TF3D);
		//mServices->patient()->insertData(image);

		DataPtr convertedData = boost::dynamic_pointer_cast<Data>(image);
		(*it) = convertedData;
	}

	/*
	if (!mConvertToUnsignedCheckBox->isChecked())
		return;

	ImagePtr image = boost::dynamic_pointer_cast<Image>(mData);
	if (!image)
		return;

	ImagePtr converted = convertImageToUnsigned(mPatientModelService, image);

	image->setVtkImageData(converted->getBaseVtkImageData());

	ImageTF3DPtr TF3D = converted->getTransferFunctions3D()->createCopy();
	ImageLUT2DPtr LUT2D = converted->getLookupTable2D()->createCopy();
	image->setLookupTable2D(LUT2D);
	image->setTransferFunctions3D(TF3D);
	mPatientModelService->insertData(image);
	*/
}

void ImportDataTypeWidget::update()
{
	this->updateParentCandidatesComboBox();

	std::map<QString, QComboBox *>::iterator it;
	for(it=mSpaceCBs.begin(); it != mSpaceCBs.end(); ++it)
	{
		QString id = it->first;
		QComboBox *box = it->second;
		this->updateSpaceComboBox(box,id);
	}
}

void ImportDataTypeWidget::prepareDataForImport()
{
	if(mShouldConvertDataToUnsigned->isChecked())
		this->applyConversionToUnsigned();

	this->importAllData();

	if(mShouldImportParentTransform->currentText() == "Yes")
		this->applyParentTransformImport();
	if(mAnatomicalCoordinateSystems->currentText() != "LPS")
		this->applyConversionLPS();
}

void ImportDataTypeWidget::showEvent(QShowEvent *event)
{
	BaseWidget::showEvent(event);
	this->update();
}

void ImportDataTypeWidget::pointMetricGroupSpaceChanged(int index)
{
	QComboBox *box = qobject_cast<QComboBox*>(QObject::sender());
	QString newSpace = box->currentData().toString();

	QString pointMetricsGroupId;
	std::map<QString, QComboBox *>::iterator it;
	for(it = mSpaceCBs.begin(); it != mSpaceCBs.end(); ++it)
	{
		if(it->second == box)
			pointMetricsGroupId = it->first;
	}
	std::vector<DataPtr> pointMetricGroup = mPointMetricGroups[pointMetricsGroupId];
	for(unsigned i=0; i<pointMetricGroup.size(); ++i)
	{
		CoordinateSystem cs(csDATA, newSpace);
		boost::dynamic_pointer_cast<PointMetric>(pointMetricGroup[i])->setSpace(cs);
	}
}

QString ImportDataTypeWidget::getInitialGuessForParentFrame()
{
	int candidateScore = 0;
	QString bestCandidate;

	for(unsigned i=0; i < mData.size(); ++i)
	{
		DataPtr data = mData[i];
		std::map<QString, QString> parentCandidates = this->getParentCandidateList();

		std::map<QString, QString>::iterator iter;
		for(iter= parentCandidates.begin(); iter != parentCandidates.end(); ++iter)
		{
			int similarity = similatiryMeasure(data->getUid(), iter->second);
			if(similarity > candidateScore && !isSegmentation(iter->second))
			{
				candidateScore = similarity;
				bestCandidate = iter->first;
			}
		}
	}

	return bestCandidate;
}

int ImportDataTypeWidget::similatiryMeasure(QString current, QString candidate)
{
	QStringList currentList = splitStringIntoSeparateParts(current);
	QStringList candidateList = splitStringIntoSeparateParts(candidate);
	return countEqualListElements(currentList, candidateList);
}

QStringList ImportDataTypeWidget::splitStringIntoSeparateParts(QString current)
{
	current = removeParenthesis(current);

	QStringList list = current.split(".", QString::SkipEmptyParts);
	QStringList list2;
	for (int i = 0; i < list.size(); ++i)
	{
		list2 << list[i].split("_", QString::SkipEmptyParts);
	}
	QStringList currentParts;
	for (int i = 0; i < list2.size(); ++i)
	{
		currentParts << list2[i].split("-", QString::SkipEmptyParts);
	}
	return currentParts;
}

QString ImportDataTypeWidget::removeParenthesis(QString current)
{
		int startParenthesis;
		do
		{
			startParenthesis = current.indexOf("{");
			int endParenthesis = current.indexOf("}");
			current.replace(startParenthesis, endParenthesis-startParenthesis+1, "");
		} while (startParenthesis != -1);
	return current;
}

int ImportDataTypeWidget::countEqualListElements(QStringList first, QStringList second)
{
	int retval = 0;
	int numComparedElements = 0;
	for (int i = 0; i < first.size(); ++i)
	{
		if(excludeElement(first[i]))
			continue;
		++numComparedElements;
		for (int j = 0; j < second.size(); ++j)
		{
			if(first[i].compare(second[j]) == 0)
			{
				++retval;
				break;//Don't count repeating elements
			}
		}
	}

	if (retval == numComparedElements)
		return 0;//Don't match equal list
	return retval;
}

bool ImportDataTypeWidget::excludeElement(QString element)
{
	if (isSegmentation(element))
		return true;
	return false;
}

void ImportDataTypeWidget::addPointMetricGroupsToTable()
{
	QString type, name;
	int groupnr = 0;
	std::map<QString, std::vector<DataPtr> >::iterator it = mPointMetricGroups.begin();
	for(; it != mPointMetricGroups.end(); ++it)
	{
		groupnr +=1;

		QString space = it->first;
		std::vector<DataPtr> datas = it->second;
		DataPtr data = datas[0];
		if(datas.empty() || !data)
		{
			continue;
		}

		QComboBox *spaceCB = new QComboBox();
		mSpaceCBs[space] = spaceCB;
		connect(spaceCB, SIGNAL(currentIndexChanged(int)), this, SLOT(pointMetricGroupSpaceChanged(int)));
		this->updateSpaceComboBox(spaceCB, space);

		type = data->getType();
		name = "Point metric group "+QString::number(groupnr);

		int newRowIndex = mTableWidget->rowCount();
		mTableWidget->setRowCount(newRowIndex+1);
		mTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem(QString::number(datas.size())));
		mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(name));
		mTableWidget->setItem(newRowIndex, 2, new QTableWidgetItem(type));
		mTableWidget->setCellWidget(newRowIndex, 3, spaceCB);
	}
}

bool ImportDataTypeWidget::isInputFileInNiftiFormat()
{
	if(mFilename.endsWith(".nii", Qt::CaseInsensitive))
		return true;
	return false;
}

bool ImportDataTypeWidget::isSegmentation(QString filename)
{
	if(filename.contains("label", Qt::CaseInsensitive))
		return true;
	if(filename.contains("seg", Qt::CaseInsensitive))
		return true;
	return false;
}

}
