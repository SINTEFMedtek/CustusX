/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxHelpIndexWidget.h"

#include <QVBoxLayout>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QLineEdit>
#include <QLabel>

#include "cxTypeConversions.h"
#include "cxHelpEngine.h"

namespace cx
{

HelpIndexWidget::HelpIndexWidget(HelpEnginePtr engine, QWidget* parent) :
	QWidget(parent),
	mEngine(engine)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	this->setLayout(layout);

	mFilterEdit = new QLineEdit;
	connect(mFilterEdit, &QLineEdit::textChanged, this, &HelpIndexWidget::filterChanged);

	layout->addWidget(new QLabel("Look for:"));
	layout->addWidget(mFilterEdit);
	layout->addWidget(mEngine->engine()->indexWidget());

	connect(mEngine->engine()->indexWidget(), &QHelpIndexWidget::linkActivated,
			this, &HelpIndexWidget::requestShowLink);
}

void HelpIndexWidget::filterChanged()
{
	mEngine->engine()->indexWidget()->filterIndices(mFilterEdit->text());
}

}//end namespace cx
