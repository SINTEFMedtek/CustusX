#include "cxToolListWidget.h"

#include <QListWidgetItem>
#include <QDir>
#include <QDropEvent>
#include <QMimeData>
#include <QAction>
#include <QMenu>
#include "cxEnumConverter.h"
#include "cxReporter.h"
#include "cxDataLocations.h"
//#include "cxToolConfigurationParser.h"
#include "cxTrackerConfiguration.h"
#include "cxLegacySingletons.h"
#include "cxToolManager.h"

namespace cx
{
//---------------------------------------------------------------------------------------------------------------------

ToolListWidget::ToolListWidget(QWidget* parent) :
		QListWidget(parent)
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
	TrackerConfigurationPtr config = toolManager()->getConfiguration();
	QString name = config->getToolName(absoluteFilePath);

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

FilteringToolListWidget::FilteringToolListWidget(QWidget* parent) :
		ToolListWidget(parent)
{
	this->setDragDropMode(QAbstractItemView::DragOnly);
	this->setDragEnabled(true);
}

FilteringToolListWidget::~FilteringToolListWidget()
{
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
	TrackerConfigurationPtr config = toolManager()->getConfiguration();
	QStringList filteredTools = config->getToolsGivenFilter(applicationsFilter,
														  trackingsystemsFilter);

	this->populate(filteredTools);
}

//---------------------------------------------------------------------------------------------------------------------

ConfigToolListWidget::ConfigToolListWidget(QWidget* parent) :
		ToolListWidget(parent)
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
	TrackerConfigurationPtr config = toolManager()->getConfiguration();

	for (int i = 0; i < this->count(); ++i)
	{
		QListWidgetItem* item = this->item(i);
		QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();

		QString toolTrackingSystem = config->getToolTrackingSystem(absoluteFilePath);

//		ToolFileParser parser(absoluteFilePath);
		QBrush brush = item->foreground();
//		QString toolTrackingSystem = enum2string(parser.getTool().mTrackerType);
		if (!trackingsystemFilter.contains(toolTrackingSystem, Qt::CaseInsensitive) || !config->verifyTool(absoluteFilePath))
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
		reporter()->sendDebug("Found no item to delete...");
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
		reporter()->sendDebug("Found no item to delete...");
		return;
	}
	mItemToDelete = item;

	connect(action, SIGNAL(triggered()), this, SLOT(deleteSlot()));
	contextMenu.addAction(action);

	contextMenu.exec(pointGlobal);
}

//---------------------------------------------------------------------------------------------------------------------
}//namespace cx
