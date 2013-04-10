// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxElastixWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include "sscLogger.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxTimedAlgorithm.h"
#include "cxPatientData.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxRegistrationDataAdapters.h"
#include "sscFileSelectWidget.h"
#include "cxDataLocations.h"
#include "cxElastixSyntaxHighlighter.h"
#include "sscCheckBoxWidget.h"
#include "cxSettings.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxElastixExecuter.h"
#include "sscStringDataAdapterXml.h"

namespace cx
{

ElastixWidget::ElastixWidget(RegistrationManagerPtr regManager, QWidget* parent) :
				RegistrationBaseWidget(regManager, parent, "ElastiXWidget", "ElastiX Registration")
{
	mElastixManager.reset(new ElastixManager(regManager));
	connect(mElastixManager.get(), SIGNAL(elastixChanged()), this, SLOT(elastixChangedSlot()));

	mRegisterButton = new QPushButton("Register");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
	mRegisterButton->setToolTip(this->defaultWhatsThis());

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("registration/elastixShowDetails").toBool());

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mElastixManager->getExecuter());

	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new RegistrationFixedImageStringDataAdapter(regManager));
	new ssc::LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new RegistrationMovingImageStringDataAdapter(regManager));
	new ssc::LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

//	ssc::StringDataAdapterXmlPtr mSettings;
//	mSettings = ssc::StringDataAdapterXml::initialize("elastixSettings", "Setting", "Current Elastix Settings", "mysettings", QStringList(), QDomNode());
	new ssc::LabeledComboBoxWidget(this, mElastixManager->getParameters()->getCurrentPreset(), entryLayout, 2);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mRegisterButton);

	this->createAction(this,
	      QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
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
	mParameterFileWidget0 = new ssc::FileSelectWidget(this);
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

	buttonsLayout->addWidget(new ssc::CheckBoxWidget(this, mElastixManager->getDisplayProcessMessages()));

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
	QDir folder(par->getParameterFilesDir());
	folder.mkpath(".");
	mParameterFileWidget0->setPath(folder.absolutePath());
	QStringList nameFilters;
	nameFilters << "*.txt";
	mParameterFileWidget0->setNameFilter(nameFilters);
	mParameterFileWidget0->setFilename(par->getActiveParameterFile0());
	std::cout << "mElastixManager->getActiveParameterFile0() " << par->getActiveParameterFile0() << std::endl;

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
