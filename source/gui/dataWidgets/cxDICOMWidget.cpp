/*
 * sscDICOMWidget.cpp
 *
 *  \date Nov 15, 2011
 *      \author christiana
 */

#include "cxDICOMWidget.h"

#ifdef SSC_USE_DCMTK

#include "sscDICOMLibAPI.h"
#include "cxImage.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxDataManager.h"
#include "cxMessageManager.h"
#include "cxTime.h"

#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx
{

DICOMWidget::DICOMWidget(QWidget* parent) :
	QWidget(parent)
{
	this->setObjectName("DICOMWidget");
	this->setWindowTitle("DICOM Import");

	mApi = DICOMLibAPI::New();
	connect(mApi.get(), SIGNAL(changed()), this, SLOT(dicomChanged()));
	connect(mApi.get(), SIGNAL(refreshStarted()), this, SLOT(refreshStartedSlot()));
	connect(mApi.get(), SIGNAL(refreshProgress(int)), this, SLOT(refreshProgressSlot(int)));
	connect(mApi.get(), SIGNAL(refreshFinished()), this, SLOT(refreshFinishedSlot()));
	mApi->setRootFolder("/home/christiana/test/Kaisa/dicom/"); // inneholder kaisa

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	QHBoxLayout* dataLayout = new QHBoxLayout;
	mRootFolderEdit = new QLineEdit(this);
	connect(mRootFolderEdit, SIGNAL(editingFinished()), this, SLOT(rootFolderEditeditingFinished()));

	mSelectRootFolderAction = new QAction(QIcon(":/icons/open.png"), tr("&Select root folder"), this);
	connect(mSelectRootFolderAction, SIGNAL(triggered()), this, SLOT(selectRootFolder()));
	mSelectRootFolderButton = new QToolButton(this);
	mSelectRootFolderButton->setDefaultAction(mSelectRootFolderAction);

	dataLayout->addWidget(mRootFolderEdit);
	dataLayout->addWidget(mSelectRootFolderButton);

	mTable = new QTableWidget(this);
	connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

	QHBoxLayout* buttonsLayout = new QHBoxLayout;

	mLoadSeriesAction = new QAction(QIcon(":/icons/open.png"), tr("&Load selected series"), this);
	connect(mLoadSeriesAction, SIGNAL(triggered()), this, SLOT(loadSeries()));
	mLoadSeriesButton = new QToolButton(this);
	mLoadSeriesButton->setDefaultAction(mLoadSeriesAction);

	mShowMetaDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Show metadata for selected series"), this);
	connect(mShowMetaDataAction, SIGNAL(triggered()), this, SLOT(showMetaData()));
	mShowMetaDataButton = new QToolButton(this);
	mShowMetaDataButton->setDefaultAction(mShowMetaDataAction);

	mProgressBar = new QProgressBar();
	mProgressBar->setMinimum(0);
	mProgressBar->setMaximum(1000);

	buttonsLayout->addWidget(mLoadSeriesButton);
	buttonsLayout->addWidget(mShowMetaDataButton);
	buttonsLayout->addWidget(mProgressBar);
	buttonsLayout->addStretch();

	topLayout->addLayout(dataLayout);
	topLayout->addWidget(mTable);
	topLayout->addLayout(buttonsLayout);

	this->dicomChanged();
}

//void DICOMWidget::setImageSavePath(QString savePath)
//{
//	mImageSavePath = savePath;
//}

void DICOMWidget::loadSeries()
{
	SeriesPtr selected = this->getSelectedSeries();
	dataManager()->loadData(selected->getImage());
	dataManager()->saveImage(selected->getImage(), cx::patientService()->getPatientData()->getActivePatientFolder());
}

void DICOMWidget::showMetaData()
{
	SeriesPtr selected = this->getSelectedSeries();
	SSC_LOG("selected %p", selected.get());
	if (!selected)
		return;
	SNW2VolumeMetaDataPtr meta = selected->getMetaData();

	std::stringstream ss;
	ss << "== Metadata for DICOM Series " << meta->mName << std::endl;
	ss << std::left;
	int hw = 20;
	ss << "(some parameters might not be set before volume is loaded)" << std::endl;

	selected->getParentStudy()->getData().put(ss);

	ss << setw(hw) << "Name: " << meta->mName << std::endl;
	ss << setw(hw) << "SeriesID: " << meta->DICOM.mSeriesID << std::endl;
	ss << setw(hw) << "SeriesDescription: " << meta->DICOM.mSeriesDescription << std::endl;
	ss << setw(hw) << "AcquisitionTime: " << meta->mAcquisitionTime.toQDateTime().toString(timestampSecondsFormatNice()) << std::endl;
	ss << setw(hw) << "ConversionTime: " << meta->mConversionTime.toQDateTime().toString(timestampSecondsFormatNice()) << std::endl;
	ss << setw(hw) << "ModalityType: " << meta->mModalityType << std::endl;
	ss << setw(hw) << "FrameOfReferenceUID: " << meta->DICOM.mFrameOfReferenceUID << std::endl;
	ss             << "rMd:\n" << meta->DICOM.m_imgMraw << std::endl;
	ss << setw(hw) << "Dim: " << meta->Volume.mDim[0] << " " << meta->Volume.mDim[1] << " " << meta->Volume.mDim[2] << std::endl;
	ss << setw(hw) << "BitsPerSample: " << meta->Volume.mBitsPerSample << std::endl;
	ss << setw(hw) << "SamplesPerPixel: " << meta->Volume.mSamplesPerPixel << std::endl;
	ss << setw(hw) << "Spacing: " << meta->Volume.mSpacing << std::endl;
	ss << setw(hw) << "FirstPixel: " << meta->Volume.mFirstPixel << std::endl;
	ss << setw(hw) << "LastPixel: " << meta->Volume.mLastPixel << std::endl;
	ss << setw(hw) << "WindowCenter: " << meta->Volume.mWindowCenter << std::endl;
	ss << setw(hw) << "WindowWidth: " << meta->Volume.mWindowWidth << std::endl;

	messageManager()->sendInfo(qstring_cast(ss.str()));
}

DICOMWidget::~DICOMWidget()
{
	// TODO Auto-generated destructor stub
}

void DICOMWidget::selectRootFolder()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Root Folder"), mApi->getRootFolder(),
		QFileDialog::ShowDirsOnly);
	if (dir.isEmpty())
		return;

	mApi->setRootFolder(dir);
}

void DICOMWidget::rootFolderEditeditingFinished()
{
	SSC_LOG("%s", "start");
	mApi->setRootFolder(mRootFolderEdit->text());
//	this->dicomChanged();
	SSC_LOG("%s", "stop");
}

void DICOMWidget::itemSelectionChanged()
{
	QString uid;
	QTableWidgetItem* item = mTable->currentItem();
	if (item)
	{
		uid = item->data(Qt::UserRole).toString();
		SSC_LOG("%s", uid.toAscii().constData());
	}

	mShowMetaDataAction->setEnabled(!uid.isEmpty());
	mLoadSeriesAction->setEnabled(!uid.isEmpty());

	//  mEditWidgets->setCurrentIndex(mTable->currentRow());
	//  enablebuttons();
}

SeriesPtr DICOMWidget::getSelectedSeries()
{
	QString uid;
	QTableWidgetItem* item = mTable->currentItem();
	if (!item)
		return SeriesPtr();

	uid = item->data(Qt::UserRole).toString();
	SSC_LOG("%s", uid.toAscii().constData());

	std::vector<StudyPtr> studies = mApi->getStudies();
	for (unsigned i=0; i<studies.size(); ++i)
	{
		std::vector<SeriesPtr> series = studies[i]->getSeries();
		for (unsigned j=0; j<series.size(); ++j)
		{
			SSC_LOG("looking at %s vs %s", series[j]->getUid().toAscii().constData(), uid.toAscii().constData());
			if (series[j]->getUid() == uid)
				return series[j];
		}
	}

	return SeriesPtr();
}

void DICOMWidget::refreshStartedSlot()
{
	SSC_LOG("SHOW");
	mProgressBar->show();
}

void DICOMWidget::refreshProgressSlot(int value)
{
	mProgressBar->setValue(value);
}

void DICOMWidget::refreshFinishedSlot()
{
	SSC_LOG("HIDE");
	mProgressBar->hide();
}


void DICOMWidget::dicomChanged()
{
	SSC_LOG("start");
	mRootFolderEdit->setText(mApi->getRootFolder());

	std::vector<StudyPtr> studies = mApi->getStudies();

	//	 	ImagePtr image = api->getStudies()[0]->getSeries()[0]->getImage();

	mTable->blockSignals(true);
    mTable->clear();

    //ready the table widget
    mTable->setColumnCount(4);
    QStringList headerItems(QStringList() << "Name" << "Modality" << "Frames" << "Date");
    mTable->setHorizontalHeaderLabels(headerItems);
    mTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    mTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTable->verticalHeader()->hide();

    int studyIndex = 0;
	for (unsigned i=0; i<studies.size(); ++i)
	{
		std::vector<SeriesPtr> series = studies[i]->getSeries();
		for (unsigned j=0; j<series.size(); ++j)
		{
		    mTable->setRowCount(studyIndex+1);
			for (unsigned k=0; k<4; ++k)
			{
		      	QTableWidgetItem* item = new QTableWidgetItem("empty");
		        item->setData(Qt::UserRole, series[j]->getUid());
//		        item->setText(series[j]->getImage()->getName());
	//	        std::cout << "write " << count  << " -- " << series[j]->getImage()->getName() << std::endl;
		        mTable->setItem(studyIndex, k, item);
			}

			mTable->item(studyIndex,0)->setText(series[j]->getMetaData()->mName);
			mTable->item(studyIndex,0)->setStatusTip(series[j]->getMetaData()->mName);
			mTable->item(studyIndex,1)->setText(series[j]->getMetaData()->mModality);
			mTable->item(studyIndex,2)->setText(qstring_cast(series[j]->getMetaData()->Volume.mDim[2]));
			mTable->item(studyIndex,3)->setText(series[j]->getMetaData()->mAcquisitionTime.toQDateTime().toString("yyyy-MM-dd hh:mm"));
			++studyIndex;
		}
	}

	mTable->blockSignals(false);

	this->itemSelectionChanged();
}

}

#endif // SSC_USE_DCMTK
