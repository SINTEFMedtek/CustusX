#include "cxImportDataTypeWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include "cxOptionsWidget.h"
#include "cxFileReaderWriterService.h"
#include "cxFileManagerService.h"
#include "cxLogger.h"
#include "cxImage.h"
#include "cxPointMetric.h"
#include "cxLabeledComboBoxWidget.h"

namespace cx
{

ImportDataTypeWidget::ImportDataTypeWidget(QWidget *parent, VisServicesPtr services, std::vector<DataPtr> data, std::vector<DataPtr> &parentCandidates) :
	BaseWidget(parent, "ImportDataTypeWidget", "Import"),
	mServices(services),
	mData(data),
	mParentCandidates(parentCandidates),
	mSelectedIndexInTable(0)
{

	//gui
	QVBoxLayout *topLayout = new QVBoxLayout(this);
	this->setLayout(topLayout);

	mAnatomicalCoordinateSystems = new QComboBox();
	mAnatomicalCoordinateSystems->addItem("LPS"); //CX
	mAnatomicalCoordinateSystems->addItem("RAS");

	mShouldImportParentTransform = new QComboBox();
	mShouldImportParentTransform->addItem("No");
	mShouldImportParentTransform->addItem("Yes");

	mParentCandidatesCB = new QComboBox();

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget(new QLabel("For all data in the file: "), 0, 0, 1, 2);
	gridLayout->addWidget(new QLabel("Specify anatomical coordinate system"), 1, 0);
	gridLayout->addWidget(mAnatomicalCoordinateSystems, 1, 1);
	gridLayout->addWidget(new QLabel("Import parents transform?"), 2, 0);
	gridLayout->addWidget(mShouldImportParentTransform, 2, 1);
	gridLayout->addWidget(new QLabel("Set parent"), 3, 0);
	gridLayout->addWidget(mParentCandidatesCB, 3, 1);

	mTableWidget = new QTableWidget();
	mTableWidget->setRowCount(0);
	mTableWidget->setColumnCount(4);
	mTableHeader<<"Type"<<"Name"<<"#"<<"Space";
	mTableWidget->setHorizontalHeaderLabels(mTableHeader);
	mTableWidget->horizontalHeader()->setStretchLastSection(true);
	mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	mTableWidget->verticalHeader()->setVisible(false);
	mTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	mTableWidget->setShowGrid(false);
	mTableWidget->setStyleSheet("QTableView {selection-background-color: red;}");
	mTableWidget->setGeometry(QApplication::desktop()->screenGeometry());

	QString type, name;
	for(int i=0; i<mData.size(); ++i)
	{
		type = mData[i]->getType();
		name = mData[i]->getName();
		QString space = mData[i]->getSpace();
		//create point metric groups
		if(type == "pointMetric")
		{
			QString space = boost::dynamic_pointer_cast<PointMetric>(mData[i])->getSpace().toString();
			(mPointMetricGroups[space]).push_back(mData[i]);
			CX_LOG_DEBUG() << "Added pmg with space " << space;
		}
		//add image or mesh directly to the table
		else
		{
			int newRowIndex = mTableWidget->rowCount();
			mTableWidget->setRowCount(newRowIndex+1);
			mTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem(type));
			mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(name));
			mTableWidget->setItem(newRowIndex, 2, new QTableWidgetItem("1"));
			mTableWidget->setItem(newRowIndex, 3, new QTableWidgetItem(space));
		}
	}
	//add point metric groups
	std::map<QString, std::vector<DataPtr> >::iterator it = mPointMetricGroups.begin();
	int groupnr = 0;
	for(it; it != mPointMetricGroups.end(); ++it)
	{
		groupnr +=1;

		QString space = it->first;
		std::vector<DataPtr> datas = it->second;
		DataPtr data = datas[0];
		if(datas.empty() || !data)
			continue;

		QComboBox *spaceCB = new QComboBox();
		QStringList parentCandidates = this->getParentCandidateList();
		foreach (QString parent, parentCandidates) {
			QVariant spaceVariant(space);
			spaceCB->addItem(parent, spaceVariant);
		}
		connect(spaceCB, SIGNAL(activated(int)), this, SLOT(pointMetricGroupSpaceChanged(int)));

		type = data->getType();
		name = "Point metric group "+QString::number(groupnr);

		int newRowIndex = mTableWidget->rowCount();
		mTableWidget->setRowCount(newRowIndex+1);
		mTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem(type));
		mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(name));
		mTableWidget->setItem(newRowIndex, 2, new QTableWidgetItem(QString::number(datas.size())));
		mTableWidget->setCellWidget(newRowIndex, 3, spaceCB);
	}

	topLayout->addLayout(gridLayout);
	topLayout->addWidget(mTableWidget);
}

QStringList ImportDataTypeWidget::getParentCandidateList()
{
	QStringList parentCandidates;
	for(int i=0; i<mParentCandidates.size(); ++i)
	{
		//TODO add QVariant with uid
		parentCandidates << mParentCandidates[i]->getUid();
	}

	return parentCandidates;
}

void ImportDataTypeWidget::showEvent(QShowEvent *event)
{
	CX_LOG_DEBUG() << "ImportDataTypeWidget is visible";
	BaseWidget::showEvent(event);

	mParentCandidatesCB->clear();
	QStringList parentCandidates = getParentCandidateList();
	mParentCandidatesCB->addItems(parentCandidates);
}

void ImportDataTypeWidget::pointMetricGroupSpaceChanged(int index)
{
	QComboBox *spaceCB = qobject_cast<QComboBox*>(QObject::sender());
	QString newSpace = spaceCB->itemText(index);
	QString pointMetricsWithSpace = spaceCB->itemData(index).toString();
	std::vector<DataPtr> pointMetricGroup = mPointMetricGroups[pointMetricsWithSpace];
	for(int i=0; i<pointMetricGroup.size(); ++i)
	{
		CoordinateSystem cs(csDATA, newSpace);
		boost::dynamic_pointer_cast<PointMetric>(pointMetricGroup[i])->setSpace(cs);
	}
}

/*
void ImportDataTypeWidget::setInitialGuessForParentFrame(DataPtr data)
{
	if(!data)
		return;

	QString base = qstring_cast(data->getName()).split(".")[0];

	std::map<QString, DataPtr> all = mServices->patient()->getDatas();
	for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
	{
		if (iter->second==data)
			continue;
		QString current = qstring_cast(iter->second->getName()).split(".")[0];
		if (base.indexOf(current)>=0)
		{
			data->get_rMd_History()->setParentSpace(iter->first);
			break;
		}
	}
}
*/

}