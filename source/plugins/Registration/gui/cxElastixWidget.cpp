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

namespace cx
{

ElastixWidget::ElastixWidget(RegistrationManagerPtr regManager, QWidget* parent) :
				RegistrationBaseWidget(regManager, parent, "ElastiXWidget", "ElastiX Registration"), mLTSRatioSpinBox(
								new QSpinBox()), mLinearCheckBox(new QCheckBox()), mRegisterButton(
								new QPushButton("Register")),
								mFilePreviewWidget(new FilePreviewWidget(this))
{
	mElastixManager.reset(new ElastixManager(regManager));

	mFilePreviewWidget->setSyntaxHighLighter<ElastixSyntaxHighlighter>();

	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	QGridLayout* layout = new QGridLayout();
	topLayout->addLayout(layout);
	topLayout->addStretch();

	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new RegistrationFixedImageStringDataAdapter(regManager));
	new ssc::LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new RegistrationMovingImageStringDataAdapter(regManager));
	new ssc::LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	layout->addLayout(entryLayout, 0, 0, 2, 2);
	layout->addWidget(mRegisterButton, 2, 0);

	layout->addWidget(new QLabel("Parameter File", this), 4, 0);
	mParameterFileWidget = new ssc::FileSelectWidget(this);
	connect(mParameterFileWidget, SIGNAL(fileSelected(QString)), this, SLOT(userParameterFileSelected(QString)));
	connect(mElastixManager.get(), SIGNAL(elastixChanged()), this, SLOT(elastixChangedSlot()));
	layout->addWidget(mParameterFileWidget, 4, 1, 1, 2);


	layout->addWidget(new QLabel("Executable", this), 5, 0);
	mExecutableEdit = new QLineEdit(this);
	connect(mExecutableEdit, SIGNAL(editingFinished()), this, SLOT(executableEditFinishedSlot()));
	layout->addWidget(mExecutableEdit, 5, 1);

	QAction* browseExecutableAction = new QAction(QIcon(":/icons/open.png"), "Browse", this);
	browseExecutableAction->setStatusTip("Select the elastiX executable");
	connect(browseExecutableAction, SIGNAL(triggered()), this, SLOT(browseExecutableSlot()));
	QToolButton* button = new QToolButton();
	button->setDefaultAction(browseExecutableAction);
	layout->addWidget(button, 5, 2);

	layout->addWidget(new ssc::CheckBoxWidget(this, mElastixManager->getDisplayProcessMessages()), 6, 0, 1, 3);

	layout->addWidget(mFilePreviewWidget, 7, 0, 1, 3);

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mElastixManager->getExecuter());
	layout->addWidget(mTimedAlgorithmProgressBar);

	this->elastixChangedSlot();
}

ElastixWidget::~ElastixWidget()
{}

QString ElastixWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>ElastiX Registration.</h3>"
      "<p>Select two datasets you want to register to eachother, and a parameter file</p>"
      "<p><i>TBD</i></p>"
      "</html>";
}

void ElastixWidget::executableEditFinishedSlot()
{
	mElastixManager->setActiveExecutable(mExecutableEdit->text());
}

void ElastixWidget::browseExecutableSlot()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Executable"), "~");
	if (fileName.isEmpty())
		return;

	mElastixManager->setActiveExecutable(fileName);
}

void ElastixWidget::userParameterFileSelected(QString filename)
{
	mElastixManager->setActiveParameterFile(filename);
}

void ElastixWidget::elastixChangedSlot()
{
	QDir folder(cx::DataLocations::getRootConfigPath() + "/elastix");
	folder.mkpath(".");
	mParameterFileWidget->setPath(folder.absolutePath());
	QStringList nameFilters;
	nameFilters << "*.txt";
	mParameterFileWidget->setNameFilter(nameFilters);
	mParameterFileWidget->setFilename(mElastixManager->getActiveParameterFile());

	mFilePreviewWidget->previewFileSlot(mElastixManager->getActiveParameterFile());

	mExecutableEdit->blockSignals(true);
	mExecutableEdit->setText(mElastixManager->getActiveExecutable());
	mExecutableEdit->blockSignals(false);
}

void ElastixWidget::registerSlot()
{
	SSC_LOG("register");
	mElastixManager->execute();
//  int lts_ratio = mLTSRatioSpinBox->value();
//  double stop_delta = 0.001; //TODO, add user interface
//  double lambda = 0; //TODO, add user interface
//  double sigma = 1.0; //TODO, add user interface
//  bool lin_flag = mLinearCheckBox->isChecked(); //TODO, add user interface
//  int sample = 1; //TODO, add user interface
//  int single_point_thre = 1; //TODO, add user interface
//  bool verbose = 1; //TODO, add user interface
//
//  ssc::messageManager()->sendDebug("Using lts_ratio: "+qstring_cast(lts_ratio));
//  QString logPath = patientService()->getPatientData()->getActivePatientFolder()+"/Logs/";
//
//  mManager->doVesselRegistration(lts_ratio, stop_delta, lambda, sigma, lin_flag, sample, single_point_thre, verbose, logPath);
}



} /* namespace cx */
