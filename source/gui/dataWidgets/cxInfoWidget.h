/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXINFOWIDGET_H_
#define CXINFOWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"

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
class cxGui_EXPORT InfoWidget : public BaseWidget
{
	Q_OBJECT

public:
	InfoWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~InfoWidget(){};

	void addStretch();

protected:
	void populateTableWidget(std::map<std::string, std::string>& info);

	QGridLayout* gridLayout;
	QTableWidget* mTableWidget;

private:
	QVBoxLayout* toptopLayout;
};

} /* namespace cx */
#endif /* CXINFOWIDGET_H_ */
