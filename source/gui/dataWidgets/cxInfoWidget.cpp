#include "cxInfoWidget.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include "sscImage.h"
#include "sscTypeConversions.h"

namespace cx {

InfoWidget::InfoWidget(QWidget* parent, QString objectName, QString windowTitle) :
		BaseWidget(parent, objectName, windowTitle)
{
	toptopLayout = new QVBoxLayout(this);
	gridLayout = new QGridLayout;
	toptopLayout->addLayout(gridLayout);

	mTabelWidget = new QTableWidget(this);
	mTabelWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

QString InfoWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3> Information</h3>"
      "<p>Displays information.</p>"
      "<p><i></i></p>"
      "</html>";
}

void InfoWidget::addStretch()
{
	toptopLayout->addStretch();
}

void InfoWidget::populateTableWidget(std::map<std::string, std::string>& info)
{
	mTabelWidget->setRowCount(info.size());
	mTabelWidget->setColumnCount(2);
	QStringList horizontalHeaders;
	horizontalHeaders << "Description";
	horizontalHeaders << "Value";
	mTabelWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mTabelWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
	mTabelWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
	mTabelWidget->setColumnWidth(0, 200);
	mTabelWidget->setColumnWidth(1, 600);

	QFont boldFont;
	boldFont.setBold(true);

	int row = 0;
	std::map<std::string, std::string>::iterator it;
	for(it = info.begin(); it != info.end(); ++it)
	{
		QTableWidgetItem *descriptionItem = new QTableWidgetItem(qstring_cast(it->first));
		descriptionItem->setFont(boldFont);
		descriptionItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mTabelWidget->setItem(row, 0, descriptionItem);

		QTableWidgetItem *valueItem = new QTableWidgetItem(qstring_cast(it->second));
		valueItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mTabelWidget->setItem(row, 1, valueItem);

		row++;
	}
}

} /* namespace cx */
