/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxEBUSCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxVector3D.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTool.h"
#include "cxTrackingService.h"
#include <cxActiveToolWidget.h>
#include "cxDoubleWidgets.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectTool.h"
#include "cxSettings.h"
#include "cxTransformFile.h"
#include "cxFrame3D.h"

namespace cx
{

//------------------------------------------------------------------------------
EBUSCalibrationWidget::EBUSCalibrationWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ebus_calibration_widget", "EBUS Calibrate"),
	mServices(services),
	mCalibrateButton(new QPushButton("Calibrate")),
	mReferencePointLabel(new QLabel("Ref. point:")),
	mCalibrationLabel(new QLabel("Calibration: \n")),
	mDeltaLabel(new QLabel("Delta:"))
{
	QVBoxLayout* toplayout = new QVBoxLayout(this);

	mCalibRefTool = StringPropertySelectTool::New(mServices->tracking());
	mCalibRefTool->setValueName("Calibration Adapter");
	mCalibRefTool->setHelp("Select Calibration Adapter sensor");

	mCalibratingTool = StringPropertySelectTool::New(mServices->tracking());
	mCalibratingTool->setValueName("Tool");
	mCalibratingTool->setHelp("Select which Tool to calibrate");

	// Adapter calibration matrix path
	mAdapterCalibrationPath = settings()->value("EBUScalibration/path").toString();

	QLabel* adapterCalibrationPathLabel = new QLabel(tr("EBUS adapter calibration matrix path:"));
	mAdapterCalibrationPathComboBox = new QComboBox();
	mAdapterCalibrationPathComboBox->addItem(mAdapterCalibrationPath);
	mAdapterCalibrationPathComboBox->setMaximumWidth(500);
	QAction* browseAdapterCalibrationPathAction = new QAction(QIcon(":/icons/open.png"), tr("Browse for EBUS adapter calibration matrix..."), this);
	connect(browseAdapterCalibrationPathAction, &QAction::triggered, this, &EBUSCalibrationWidget::browseAdapterCalibrationPathSlot);
	QToolButton* browseAdapterCalibrationPathButton = new QToolButton(this);
	browseAdapterCalibrationPathButton->setDefaultAction(browseAdapterCalibrationPathAction);

	QGridLayout* gridLayout = new QGridLayout();

	gridLayout->addWidget(adapterCalibrationPathLabel, 0, 0);
	gridLayout->addWidget(mAdapterCalibrationPathComboBox, 1, 0);
	gridLayout->addWidget(browseAdapterCalibrationPathButton, 1, 1);

	toplayout->addWidget(mReferencePointLabel);
	toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibRefTool));
	toplayout->addWidget(new LabeledComboBoxWidget(this, mCalibratingTool));
	toplayout->addLayout(gridLayout);
	toplayout->addWidget(mCalibrateButton);
	toplayout->addWidget(mCalibrationLabel);
	toplayout->addWidget(this->createHorizontalLine());
	toplayout->addStretch();

	mReferencePointLabel->setText("<b>EBUS calibration adapter - Use sensor on adapter as reference</b>");

	connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));

	connect(mCalibRefTool.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

	//setting default state
	this->toolSelectedSlot();

	connect(mServices->tracking().get(), &cx::TrackingService::stateChanged, this, &EBUSCalibrationWidget::trackingStartedSlot);
}

EBUSCalibrationWidget::~EBUSCalibrationWidget()
{}

void EBUSCalibrationWidget::calibrateSlot()
{
	ToolPtr refTool = mCalibRefTool->getTool();
	ToolPtr tool = mCalibratingTool->getTool();
	if(!refTool || !tool)
	{
	reportError(QString("Calibration prerequisited not met: calref:%1, tool:%2").arg(refTool!=0).arg(tool!=0) );
	return;
	}
	if(!refTool->getVisible() || !tool->getVisible())
	{
		reportError(QString("Calibration prerequisited not met: calref vis:%1, tool vis :%2").arg(refTool->getVisible()).arg(tool->getVisible()) );
		return;
	}

	EBUSCalibrationCalculator calc(tool, refTool);
	Transform3D adapterCalibration = readCalibrationFile(mAdapterCalibrationPath);
	Transform3D calibration = calc.get_calibration_sMt(adapterCalibration);

	QMessageBox msgBox;
	msgBox.setText("Do you want to overwrite "+tool->getName()+"'s calibration file?");
	msgBox.setInformativeText("This cannot be undone.");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();

	if(ret == QMessageBox::Ok)
	{
		try
		{
			tool->setCalibration_sMt(calibration);
		}
		catch(std::exception& e)
		{
			QMessageBox msgBox2;
			msgBox2.setText("Unknown error, could not calibrate the tool: "+tool->getName()+".");
			msgBox2.setInformativeText(QString(e.what()));
			msgBox2.setStandardButtons(QMessageBox::Ok);
			msgBox2.setDefaultButton(QMessageBox::Ok);
			int ret2 = msgBox2.exec();
			return;
		}
		mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
	}
}

void EBUSCalibrationWidget::toolSelectedSlot()
{
	//  QString text("Ref. point: <UNDEFINED POINT>");
	mCalibrateButton->setEnabled(false);

	if (mCalibRefTool->getTool())
	{
//		mCalibrationLabel->setText("Calibration:\n" + qstring_cast(mCalibratingTool->getTool()->getCalibration_sMt()));
//    Transform3D calibration = mCalibratingTool->getTool()->getCalibration_sMt();
//    mCalibrationLabel->setText(QString("Calibration matrix for %1:\n%2").arg(tool->getName(), qstring_cast(calibration)));
		mCalibrateButton->setEnabled(true);
	}

	//  mReferencePointLabel->setText(text);
}

void EBUSCalibrationWidget::browseAdapterCalibrationPathSlot()
{
	QFileInfo fileInfo(mAdapterCalibrationPath);
	mAdapterCalibrationPath = QFileDialog::getOpenFileName(this, tr("Find adapter calibration matrix file"), fileInfo.absolutePath());

	settings()->setValue("EBUScalibration/path", mAdapterCalibrationPath);

	if(!mAdapterCalibrationPath.isEmpty())
	{
		mAdapterCalibrationPathComboBox->addItem( mAdapterCalibrationPath );
		mAdapterCalibrationPathComboBox->setCurrentIndex( mAdapterCalibrationPathComboBox->currentIndex() + 1 );
	}
}

Transform3D EBUSCalibrationWidget::readCalibrationFile(QString absoluteFilePath)
{
	bool ok = true;
	TransformFile file(absoluteFilePath);
	Transform3D retval = file.read(&ok);

	if (ok)
		retval = Frame3D::create(retval).transform(); // clean rotational parts, transform should now be pure rotation+translation

	return retval;
}


//------------------------------------------------------------------------------

void EBUSCalibrationWidget::trackingStartedSlot()
{
	ToolPtr ref = mServices->tracking()->getTool("calibration_tool");
	if (ref)
		mCalibRefTool->setValue(ref->getUid());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------




EBUSCalibrationCalculator::EBUSCalibrationCalculator(ToolPtr tool, ToolPtr calRef) :
	mTool(tool), mCalibrationRef(calRef)
{
	m_sMpr = mTool->getCalibration_sMt() * mTool->get_prMt().inv();

	m_qMcr = Transform3D::Identity();
	m_qMpr = m_qMcr * mCalibrationRef->get_prMt().inv();
}

Transform3D EBUSCalibrationCalculator::get_calibration_sMt(Transform3D adapterCalibration)
{
	Transform3D tool_prMs = mTool->get_prMt() * mTool->getCalibration_sMt().inverse();

	Transform3D calibration = tool_prMs.inverse() * adapterCalibration;

	return calibration;
}

}
