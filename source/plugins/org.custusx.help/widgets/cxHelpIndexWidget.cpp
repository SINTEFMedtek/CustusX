/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
