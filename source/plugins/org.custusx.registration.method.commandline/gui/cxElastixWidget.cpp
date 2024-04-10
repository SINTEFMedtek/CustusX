/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxSelectDataStringProperty.h"

namespace cx
{

ElastixWidget::ElastixWidget(RegServicesPtr services, QWidget* parent) :
	RegistrationBaseWidget(services, parent, "org_custusx_registration_method_commandline_elastix_widget", "ElastiX Registration"),
	mRegisterButton(nullptr),
	mParameterFileWidget0(nullptr),
	mParameterFileWidget1(nullptr),
	mParameterFileWidget2(nullptr),
	mFilePreviewWidget(nullptr),
	mTimedAlgorithmProgressBar(nullptr),
	mOptionsWidget(nullptr)
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

	int row = 0;
	mFixedImage.reset(new StringPropertyRegistrationFixedImage(mServices->registration(), mServices->patient()));
	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, row++);

	StringPropertyRegistrationMovingImagePtr movingImage = StringPropertyRegistrationMovingImagePtr(new StringPropertyRegistrationMovingImage(mServices->registration(), mServices->patient()));
	movingImage->setValueName("Moving image");
	mMovingImage = movingImage;
	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, row++);

	//Add PET image to be deformed by registration result
	mDeformImage = StringPropertySelectImage::New(mServices->patient());
	mDeformImage->setValueName("Moving/deforming image 2");
	mDeformImage->setHelp("Select image to be registered and deformed in the same way as the moving image above");
	sscCreateDataWidget(this, mDeformImage, entryLayout, row++);
	connect(mDeformImage.get(), &SelectDataStringPropertyBase::dataChanged, this, &ElastixWidget::deformImageChanged);

	new LabeledComboBoxWidget(this, mElastixManager->getParameters()->getCurrentPreset(), entryLayout, row++);

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

	mParameterFileWidget0 = new FileSelectWidget(this);
	mParameterFileWidget1 = new FileSelectWidget(this);
	mParameterFileWidget2 = new FileSelectWidget(this);
	layout->addWidget(new QLabel("Parameter File 1", this), ++line, 0);
	layout->addWidget(mParameterFileWidget0, line, 1, 1, 2);
	layout->addWidget(new QLabel("Parameter File 2", this), ++line, 0);
	layout->addWidget(mParameterFileWidget1, line, 1, 1, 2);
	layout->addWidget(new QLabel("Parameter File 3", this), ++line, 0);
	layout->addWidget(mParameterFileWidget2, line, 1, 1, 2);
	connect(mParameterFileWidget0, &FileSelectWidget::fileSelected, this, &ElastixWidget::userParameterFile0Selected);
	connect(mParameterFileWidget1, &FileSelectWidget::fileSelected, this, &ElastixWidget::userParameterFile1Selected);
	connect(mParameterFileWidget2, &FileSelectWidget::fileSelected, this, &ElastixWidget::userParameterFile2Selected);
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

void ElastixWidget::userParameterFile0Selected(QString filename)
{
	mElastixManager->getParameters()->getActiveParameterFile0()->setValue(filename);
}
void ElastixWidget::userParameterFile1Selected(QString filename)
{
	mElastixManager->getParameters()->getActiveParameterFile1()->setValue(filename);
}
void ElastixWidget::userParameterFile2Selected(QString filename)
{
	mElastixManager->getParameters()->getActiveParameterFile2()->setValue(filename);
}
void ElastixWidget::deformImageChanged(QString imageUid)
{
	mElastixManager->getParameters()->setDeformImage(imageUid);
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
	EmbeddedFilepath par1 = par->getActiveParameterFile1()->getEmbeddedPath();
	EmbeddedFilepath par2 = par->getActiveParameterFile2()->getEmbeddedPath();
	QStringList folders = par0.getRootPaths();
	QStringList parfolders;
	for (int i=0; i<folders.size(); ++i)
		this->recurseParameterFolders(folders[i], &parfolders);

	mParameterFileWidget0->setPaths(parfolders);
	mParameterFileWidget1->setPaths(parfolders);
	mParameterFileWidget2->setPaths(parfolders);
	QStringList nameFilters;
	nameFilters << "*";
	mParameterFileWidget0->setNameFilter(nameFilters);
	mParameterFileWidget1->setNameFilter(nameFilters);
	mParameterFileWidget2->setNameFilter(nameFilters);
	mParameterFileWidget0->setFilename(par0.getAbsoluteFilepath());
	mParameterFileWidget1->setFilename(par1.getAbsoluteFilepath());
	mParameterFileWidget2->setFilename(par2.getAbsoluteFilepath());


	mFilePreviewWidget->previewFileSlot(par0.getAbsoluteFilepath());
}

void ElastixWidget::registerSlot()
{
	mElastixManager->execute();
}



} /* namespace cx */
