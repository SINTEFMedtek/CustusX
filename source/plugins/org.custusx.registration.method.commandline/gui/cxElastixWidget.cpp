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
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxRegistrationDataAdapters.h"
#include "cxFileSelectWidget.h"
#include "cxDataLocations.h"
#include "cxElastixSyntaxHighlighter.h"
#include "cxCheckBoxWidget.h"
#include "cxSettings.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxElastixExecuter.h"
#include "cxStringDataAdapterXml.h"
#include "cxLogicManager.h"

namespace cx
{

ElastixWidget::ElastixWidget(regServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "ElastiXWidget", "ElastiX Registration")
{
	mElastixManager.reset(new ElastixManager(services));
	connect(mElastixManager.get(), SIGNAL(elastixChanged()), this, SLOT(elastixChangedSlot()));

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->setMargin(0);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("registration/elastixShowDetails").toBool());

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mElastixManager->getExecuter());

	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new RegistrationFixedImageStringDataAdapter(services.registrationService, services.patientModelService));
	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new RegistrationMovingImageStringDataAdapter(services.registrationService, services.patientModelService));
	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

//	StringDataAdapterXmlPtr mSettings;
//	mSettings = StringDataAdapterXml::initialize("elastixSettings", "Setting", "Current Elastix Settings", "mysettings", QStringList(), QDomNode());
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
//	topLayout->addSpacing(topLayout->spacing()*2);
//	topLayout->addWidget(this->createHorizontalLine());
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

	layout->addWidget(new QLabel("Executable", this), line, 0);
	mExecutableEdit = new QLineEdit(this);
	connect(mExecutableEdit, SIGNAL(editingFinished()), this, SLOT(executableEditFinishedSlot()));
	layout->addWidget(mExecutableEdit, line, 1);

	QAction* browseExecutableAction = new QAction(QIcon(":/icons/open.png"), "Browse", this);
	browseExecutableAction->setStatusTip("Select the elastiX executable");
	connect(browseExecutableAction, SIGNAL(triggered()), this, SLOT(browseExecutableSlot()));
	QToolButton* button = new QToolButton();
	button->setDefaultAction(browseExecutableAction);
	layout->addWidget(button, line, 2);
	++line;

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	layout->addLayout(buttonsLayout, line, 0, 1, 3);

	buttonsLayout->addWidget(new CheckBoxWidget(this, mElastixManager->getDisplayProcessMessages()));

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

QString ElastixWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>ElastiX Registration.</h3>"
      "<p>Select two datasets you want to register to each other, "
      "and a preset suitable for your images.</p>"
	  "<p><a http://elastix.isi.uu.nl/> ElastiX </a> is normally used for the registration "
	  "as an external application, although any program with the same input/output "
	  "can be used. Add parameter files to the folder config/elastix in order to show "
	  "them to CustusX.</p>"
      "<p>If a nonlinear registration is selected, CustusX will attempt to import the "
      "volume produced by ElastiX.</p>"
      "</html>";
}

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

void ElastixWidget::executableEditFinishedSlot()
{
	mElastixManager->getParameters()->setActiveExecutable(mExecutableEdit->text());
}

void ElastixWidget::browseExecutableSlot()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Executable"), "~");
	if (fileName.isEmpty())
		return;

	mElastixManager->getParameters()->setActiveExecutable(fileName);
}

void ElastixWidget::userParameterFileSelected(QString filename)
{
	mElastixManager->getParameters()->setActiveParameterFile0(filename);
}

void ElastixWidget::elastixChangedSlot()
{
	ElastixParametersPtr par = mElastixManager->getParameters();
	QStringList folders = par->getParameterFilesDir();
	for (int i=0; i<folders.size(); ++i)
	{
		QDir folder(folders[i]);
		folder.mkpath(".");
	}

	mParameterFileWidget0->setPaths(folders);
	QStringList nameFilters;
	nameFilters << "*.*";
	mParameterFileWidget0->setNameFilter(nameFilters);
	mParameterFileWidget0->setFilename(par->getActiveParameterFile0());

	mFilePreviewWidget->previewFileSlot(par->getActiveParameterFile0());

	mExecutableEdit->blockSignals(true);
	mExecutableEdit->setText(par->getActiveExecutable());
	mExecutableEdit->blockSignals(false);
}

void ElastixWidget::registerSlot()
{
	mElastixManager->execute();
}



} /* namespace cx */
