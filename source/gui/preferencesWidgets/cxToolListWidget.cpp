/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolListWidget.h"

#include <QListWidgetItem>
#include <QDir>
#include <QDropEvent>
#include <QMimeData>
#include <QAction>
#include <QMenu>
#include <QDrag>
#include "cxEnumConverter.h"
#include "cxLogger.h"
#include "cxTrackerConfiguration.h"
#include "cxTrackingService.h"

namespace cx
{
//---------------------------------------------------------------------------------------------------------------------

ToolListWidget::ToolListWidget(TrackingServicePtr trackingService, QWidget* parent) :
	QListWidget(parent),
	mTrackingService(trackingService)
{
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChangedSlot()));

	this->setSelectionBehavior(QAbstractItemView::SelectItems);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
}

ToolListWidget::~ToolListWidget()
{
}

void ToolListWidget::populate(QStringList toolsAbsoluteFilePath)
{
	this->clear();

	foreach(QString tool, toolsAbsoluteFilePath)
	{
		this->addTool(tool);
	}
	emit listSizeChanged();
}

void ToolListWidget::addTool(QString absoluteFilePath)
{
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	QString name = config->getTool(absoluteFilePath).mName;

//	QFile file(absoluteFilePath);
//	QFileInfo info(file);
//	QListWidgetItem* item = new QListWidgetItem(/*QIcon, */info.dir().dirName());
	QListWidgetItem* item = new QListWidgetItem(name);
	item->setData(Qt::ToolTipRole, absoluteFilePath);
	item->setData(Qt::UserRole, absoluteFilePath);
	this->addItem(item);
	emit listSizeChanged();
}

void ToolListWidget::selectionChangedSlot()
{
	QListWidgetItem* selectedItem = this->currentItem();
	this->toolSelectedSlot(selectedItem);
}

void ToolListWidget::toolSelectedSlot(QListWidgetItem* item)
{
	if (!item)
		return;

	QString absoluteFilePath = item->data(Qt::UserRole).toString();
	emit toolSelected(absoluteFilePath);
}

//---------------------------------------------------------------------------------------------------------------------

FilteringToolListWidget::FilteringToolListWidget(TrackingServicePtr trackingService, QWidget* parent) :
		ToolListWidget(trackingService, parent)
{
	this->setDragDropMode(QAbstractItemView::DragOnly);
	this->setDragEnabled(true);
}

FilteringToolListWidget::~FilteringToolListWidget()
{
}

QSize FilteringToolListWidget::minimumSizeHint() const
{
	QFontMetrics metric(this->font());
	int height = metric.lineSpacing() * 15; // approx 15 lines of text
	return QSize(300,height); // the height here is important: the default is 150, which is too little
}

void FilteringToolListWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		startPos = event->pos();
	QListWidget::mousePressEvent(event);
}

void FilteringToolListWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		int distance = (event->pos() - startPos).manhattanLength();
		if (distance >= 10)
			this->startDrag();
	}
}

void FilteringToolListWidget::startDrag()
{
	QListWidgetItem *item = currentItem();
	if (item)
	{
		QMimeData *mimeData = new QMimeData;
		mimeData->setText(item->data(Qt::ToolTipRole).toString());
		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);

		drag->start(Qt::MoveAction);
	}
}

void FilteringToolListWidget::filterSlot(QStringList applicationsFilter, QStringList trackingsystemsFilter)
{
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	QStringList filteredTools = config->getToolsGivenFilter(applicationsFilter,
														  trackingsystemsFilter);
//	filteredTools.sort(); // no good: we would like to sort on name, but the list is full paths
	this->populate(filteredTools);
}

//---------------------------------------------------------------------------------------------------------------------

ConfigToolListWidget::ConfigToolListWidget(TrackingServicePtr trackingService, QWidget* parent) :
		ToolListWidget(trackingService, parent)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);

	this->viewport()->setAcceptDrops(true);
	this->setDropIndicatorShown(true);
	this->setDefaultDropAction(Qt::CopyAction);
	this->setDragDropMode(QAbstractItemView::DropOnly);

connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuSlot(const QPoint &)));
}

ConfigToolListWidget::~ConfigToolListWidget()
{
}

void ConfigToolListWidget::dragEnterEvent(QDragEnterEvent *event)
{
	QStringList all = this->getTools();
	if (all.contains(event->mimeData()->text()))
		event->ignore();
	else
		event->accept();
}

void ConfigToolListWidget::dragMoveEvent(QDragMoveEvent *event)
{
	event->setDropAction(Qt::MoveAction);
//  event->accept();
	QStringList all = this->getTools();
	if (all.contains(event->mimeData()->text()))
		event->ignore();
	else
		event->accept();
}

void ConfigToolListWidget::dropEvent(QDropEvent *event)
{
//  std:: cout << "received dropEvent: " << event->mimeData()->text() << std::endl;
	this->addTool(event->mimeData()->text());
//  addItem(event->mimeData()->text());
	event->setDropAction(Qt::MoveAction);
	event->accept();
	emit userChangedList();
	emit listSizeChanged();
}

QStringList ConfigToolListWidget::getTools()
{
	QStringList retval;

	for (int i = 0; i < this->count(); ++i)
	{
		QListWidgetItem* item = this->item(i);
		retval << item->data(Qt::ToolTipRole).toString();
	}

	return retval;
}

void ConfigToolListWidget::configSlot(QStringList toolsAbsoluteFilePath)
{
	this->populate(toolsAbsoluteFilePath);
}

void ConfigToolListWidget::filterSlot(QStringList trackingsystemFilter)
{
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();

	for (int i = 0; i < this->count(); ++i)
	{
		QListWidgetItem* item = this->item(i);
		QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();

		QString toolTrackingSystemName = config->getTool(absoluteFilePath).mTrackingSystemName;

		QBrush brush = item->foreground();
		if (!trackingsystemFilter.contains(toolTrackingSystemName, Qt::CaseInsensitive) || !config->verifyTool(absoluteFilePath))
			brush.setColor(Qt::red);
		else
			brush.setColor(Qt::black);

		item->setForeground(brush);
	}
}

void ConfigToolListWidget::deleteSlot()
{
	if (!mItemToDelete)
	{
		reportDebug("Found no item to delete...");
		return;
	}
	this->deleteItemSlot(mItemToDelete);
}

void ConfigToolListWidget::deleteItemSlot(QListWidgetItem* item)
{
	delete item;
	emit userChangedList();
	emit listSizeChanged();
}

void ConfigToolListWidget::contextMenuSlot(const QPoint& point)
{
	QWidget* sender = dynamic_cast<QWidget*>(this->sender());
	QPoint pointGlobal = sender->mapToGlobal(point);
	QMenu contextMenu(sender);

	QAction* action = new QAction("Remove", &contextMenu);

	QListWidgetItem* item = this->itemAt(point);
	if (!item)
	{
		reportDebug("Found no item to delete...");
		return;
	}
	mItemToDelete = item;

	connect(action, SIGNAL(triggered()), this, SLOT(deleteSlot()));
	contextMenu.addAction(action);

	contextMenu.exec(pointGlobal);
}

//---------------------------------------------------------------------------------------------------------------------
}//namespace cx
