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

	mTableWidget = new QTableWidget(this);
	mTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
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
	mTableWidget->setRowCount(info.size());
	mTableWidget->setColumnCount(2);
	QStringList horizontalHeaders;
	horizontalHeaders << "Description";
	horizontalHeaders << "Value";
	mTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mTableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
	mTableWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
	mTableWidget->setColumnWidth(0, 200);
	mTableWidget->setColumnWidth(1, 600);

	QFont boldFont;
	boldFont.setBold(true);

	int row = 0;
	std::map<std::string, std::string>::iterator it;
	for(it = info.begin(); it != info.end(); ++it)
	{
		QTableWidgetItem *descriptionItem = new QTableWidgetItem(qstring_cast(it->first));
		descriptionItem->setFont(boldFont);
		descriptionItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mTableWidget->setItem(row, 0, descriptionItem);

		QTableWidgetItem *valueItem = new QTableWidgetItem(qstring_cast(it->second));
		valueItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mTableWidget->setItem(row, 1, valueItem);

		row++;
	}
}

} /* namespace cx */
