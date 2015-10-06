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

#include "cxElastixWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>

#include "cxTypeConversions.h"

#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationProperties.h"
#include "cxFileSelectWidget.h"
#include "cxElastixSyntaxHighlighter.h"
#include "cxCheckBoxWidget.h"
#include "cxSettings.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxElastixExecuter.h"
#include "cxStringProperty.h"
#include "cxFilePathProperty.h"
#include "cxDataLocations.h"
#include "cxHelperWidgets.h"

namespace cx
{

ElastixWidget::ElastixWidget(RegServicesPtr services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_commandline_elastix_widget", "ElastiX Registration"),
	mRegisterButton(NULL),
	mParameterFileWidget0(NULL),
	mFilePreviewWidget(NULL),
	mTimedAlgorithmProgressBar(NULL),
	mOptionsWidget(NULL)
{
	this->setModified();
}

void ElastixWidget::prePaintEvent()
{
	if (!mElastixManager)
	{
		this->createUI();
	}
}

void ElastixWidget::createUI()
{
	mElastixManager.reset(new ElastixManager(mServices));
	connect(mElastixManager.get(), SIGNAL(elastixChanged()), this, SLOT(elastixChangedSlot()));

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->setMargin(0);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("registration/elastixShowDetails").toBool());

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mElastixManager->getExecuter());

	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(mServices->registration(), mServices->patient()));
	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new StringPropertyRegistrationMovingImage(mServices->registration(), mServices->patient()));
	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	new LabeledComboBoxWidget(this, mElastixManager->getParameters()->getCurrentPreset(), entryLayout, 2);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mRegisterButton);

	this->createAction(this,
		  QIcon(":/icons/open_icon_library/system-run-5.png"),
		  "Details", "Show Elastix Settings Details",
		  SLOT(toggleDetailsSlot()),
		  buttonsLayout);

	topLayout->addLayout(entryLayout);
	topLayout->addLayout(buttonsLayout);
	topLayout->addWidget(mOptionsWidget, 1);
	topLayout->addStretch();
	topLayout->addWidget(mTimedAlgorithmProgressBar);

	this->elastixChangedSlot();
}

void ElastixWidget::toggleDetailsSlot()
{
  mOptionsWidget->setVisible(!mOptionsWidget->isVisible());
  settings()->setValue("registration/elastixShowDetails", mOptionsWidget->isVisible());
}

QWidget* ElastixWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);

	int line = 0;

	layout->addWidget(this->createHorizontalLine(), line, 0, 1, 3);
	++line;

	layout->addWidget(new QLabel("Parameter File", this), line, 0);
	mParameterFileWidget0 = new FileSelectWidget(this);
	connect(mParameterFileWidget0, SIGNAL(fileSelected(QString)), this, SLOT(userParameterFileSelected(QString)));
	layout->addWidget(mParameterFileWidget0, line, 1, 1, 2);
	++line;

	QWidget* executableWidget = sscCreateDataWidget(this, mElastixManager->getParameters()->getActiveExecutable());
	layout->addWidget(executableWidget, line, 0, 1, 3);
	++line;

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	layout->addLayout(buttonsLayout, line, 0, 1, 3);

	buttonsLayout->addWidget(new CheckBoxWidget(this, mElastixManager->getDisplayProcessMessages()));
	buttonsLayout->addWidget(new CheckBoxWidget(this, mElastixManager->getDisableRendering()));

	this->createAction(this,
	                QIcon(":/icons/preset_remove.png"),
					"Delete the current preset", "",
	                SLOT(deletePresetSlot()),
	                buttonsLayout);

	this->createAction(this,
		            QIcon(":/icons/preset_save.png"),
					"Add the current setting as a preset", "",
	                SLOT(savePresetSlot()),
	                buttonsLayout);

	++line;

	mFilePreviewWidget = new FilePreviewWidget(this);
	mFilePreviewWidget->setSyntaxHighLighter<ElastixSyntaxHighlighter>();
	mFilePreviewWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	layout->addWidget(mFilePreviewWidget, line, 0, 1, 3);
	++line;

	return retval;
}

ElastixWidget::~ElastixWidget()
{}


void ElastixWidget::savePresetSlot()
{
	ElastixParametersPtr par = mElastixManager->getParameters();

	QString newName = par->getPresetNameSuggesion();

    bool ok;
    QString text = QInputDialog::getText(this, "Save Preset",
                                         "Custom Preset Name", QLineEdit::Normal,
                                         newName, &ok);
    if (!ok || text.isEmpty())
      return;

	par->saveCurrentPreset(text);
}

void ElastixWidget::deletePresetSlot()
{
	mElastixManager->getParameters()->removeCurrentPreset();
}

void ElastixWidget::userParameterFileSelected(QString filename)
{
	mElastixManager->getParameters()->getActiveParameterFile0()->setValue(filename);
}

void ElastixWidget::recurseParameterFolders(QString root, QStringList* retval)
{
	QDir folder(root);
	folder.setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
	QFileInfoList info = folder.entryInfoList();
	for (int i=0; i<info.size(); ++i)
	{
		QString current = info[i].absoluteFilePath();
		if (current.endsWith("/par"))
		{
			retval->append(current);
		}

		this->recurseParameterFolders(current, retval);
	}
}

void ElastixWidget::elastixChangedSlot()
{
	ElastixParametersPtr par = mElastixManager->getParameters();
	EmbeddedFilepath par0 = par->getActiveParameterFile0()->getEmbeddedPath();
	QStringList folders = par0.getRootPaths();
	QStringList parfolders;
	for (int i=0; i<folders.size(); ++i)
		this->recurseParameterFolders(folders[i], &parfolders);

	mParameterFileWidget0->setPaths(parfolders);
	QStringList nameFilters;
	nameFilters << "*";
	mParameterFileWidget0->setNameFilter(nameFilters);
	mParameterFileWidget0->setFilename(par0.getAbsoluteFilepath());


	mFilePreviewWidget->previewFileSlot(par0.getAbsoluteFilepath());
}

void ElastixWidget::registerSlot()
{
	mElastixManager->execute();
}



} /* namespace cx */
