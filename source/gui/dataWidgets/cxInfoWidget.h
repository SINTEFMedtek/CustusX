#ifndef CXINFOWIDGET_H_
#define CXINFOWIDGET_H_

#include "cxBaseWidget.h"
#include "sscForwardDeclarations.h"

class QGridLayout;
class QVBoxLayout;
class QTableWidget;

namespace cx {

/*
 * \class InfoWidget
 *
 * \brief Base class for displaying information.
 *
 * \date Apr 26, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class InfoWidget : public BaseWidget
{
	Q_OBJECT

public:
	InfoWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~InfoWidget(){};

	virtual QString defaultWhatsThis() const;

	void addStretch();

protected:
	void populateTableWidget(std::map<std::string, std::string>& info);

	QGridLayout* gridLayout;
	QTableWidget* mTabelWidget;

private:
	QVBoxLayout* toptopLayout;
};

} /* namespace cx */
#endif /* CXINFOWIDGET_H_ */
