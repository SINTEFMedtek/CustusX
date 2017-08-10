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
			space = boost::dynamic_pointer_cast<PointMetric>(mData[i])->getSpace().toString();
			if(space.isEmpty())
				CX_LOG_WARNING() << "Trying to add empty space";
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
		mTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem(type));
		mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(name));
		mTableWidget->setItem(newRowIndex, 2, new QTableWidgetItem(QString::number(datas.size())));
		mTableWidget->setCellWidget(newRowIndex, 3, spaceCB);
	}

	topLayout->addLayout(gridLayout);
	topLayout->addWidget(mTableWidget);
}

//QStringList ImportDataTypeWidget::getParentCandidateList()
std::map<QString, QString> ImportDataTypeWidget::getParentCandidateList()
{
	std::map<QString, QString> parentCandidates;
	for(int i=0; i<mParentCandidates.size(); ++i)
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
	mParentCandidatesCB->clear();
	std::map<QString, QString> parentCandidates = this->getParentCandidateList();
	std::map<QString, QString>::iterator it;
	for(it= parentCandidates.begin(); it != parentCandidates.end(); ++it)
	{
		QVariant id(it->second);
		mParentCandidatesCB->addItem(it->first, id);
	}
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