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

#include "cxPC_ICP_Widget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <vtkCellArray.h>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationProperties.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMesh.h"
#include "cxView.h"
#include "cxGeometricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"

namespace cx
{

PCICPWidget::PCICPWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "org_custusx_registration_method_pc_icpwidget", "Surface-Tracker Registration"),
	mLTSRatioSpinBox(new QSpinBox()), mLinearCheckBox(new QCheckBox()), mAutoLTSCheckBox(new QCheckBox()),
	mRegisterButton(new QPushButton("Register")),
	mServices(services)
{
	mRegisterButton->setEnabled(false);
	connect(mRegisterButton, &QPushButton::clicked, this, &PCICPWidget::registerSlot);

//	connect(mServices.registrationService.get(), &RegistrationService::fixedDataChanged,
//			this, &PCICPWidget::inputChanged);
//	connect(mServices.registrationService.get(), &RegistrationService::movingDataChanged,
//			this, &PCICPWidget::inputChanged);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	QGridLayout* layout = new QGridLayout();
	topLayout->addLayout(layout);

	mVesselRegOptionsButton = new QPushButton("Options", this);
	mVesselRegOptionsButton->setEnabled(false);
	mVesselRegOptionsButton->setCheckable(true);

	mVesselRegOptionsWidget = this->createGroupbox(this->createOptionsWidget(),
		"Vessel registration options");
	connect(mVesselRegOptionsButton, SIGNAL(clicked(bool)), mVesselRegOptionsWidget, SLOT(setVisible(bool)));
	mVesselRegOptionsWidget->setVisible(mVesselRegOptionsButton->isChecked());

	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

//	mFixedImage.reset(new StringPropertyRegistrationFixedImage(services.registrationService, services.patientModelService));
//	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
//	mMovingImage.reset(new StringPropertyRegistrationMovingImage(services.registrationService, services.patientModelService));
//	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	layout->addLayout(entryLayout, 0, 0, 2, 2);
	layout->addWidget(mRegisterButton, 2, 0);
	layout->addWidget(mVesselRegOptionsButton, 2, 1);
	layout->addWidget(mVesselRegOptionsWidget, 3, 0, 1, 2);
}

PCICPWidget::~PCICPWidget()
{
}

void PCICPWidget::inputChanged()
{
	if(mMoving && mFixed)
	{
		mRegisterButton->setEnabled(true);
		mVesselRegOptionsButton->setEnabled(true);
		mVesselRegOptionsWidget->setVisible(mVesselRegOptionsButton->isChecked());
	}
	else
	{
		mRegisterButton->setEnabled(false);
		mVesselRegOptionsButton->setEnabled(false);
		mVesselRegOptionsWidget->setVisible(false);
	}
}

void PCICPWidget::registerSlot()
{
	/*
//	int lts_ratio = mLTSRatioSpinBox->value();
//	double stop_delta = 0.001; //TODO, add user interface
//	double lambda = 0; //TODO, add user interface
//	double sigma = 1.0; //TODO, add user interface
//	bool lin_flag = mLinearCheckBox->isChecked(); //TODO, add user interface
//	int sample = 1; //TODO, add user interface
//	int single_point_thre = 1; //TODO, add user interface
//	bool verbose = 1; //TODO, add user interface

	QString logPath = mServices->patientModelService->getActivePatientFolder() + "/Logs/";

//	mRegistrationService->doVesselRegistration(lts_ratio, stop_delta, lambda, sigma, lin_flag, sample, single_point_thre, verbose,
//		logPath);

	SeansVesselReg vesselReg;
	vesselReg.mt_auto_lts = true;
	vesselReg.mt_ltsRatio = mLTSRatioSpinBox->value();
	vesselReg.mt_doOnlyLinear = mLinearCheckBox->isChecked();
	vesselReg.mt_auto_lts = mAutoLTSCheckBox->isChecked();

	if (vesselReg.mt_auto_lts)
	{
		reportDebug("Using automatic lts_ratio");
	}
	else
	{
		reportDebug("Using lts_ratio: " + qstring_cast(vesselReg.mt_ltsRatio));
	}

	if(!mMoving)
	{
		reportWarning("Moving volume not set.");
		return;
	}
	else if(!mFixed)
	{
		reportWarning("Fixed volume not set.");
		return;
	}

	bool success = vesselReg.execute(mMoving, mFixed, logPath);
	if (!success)
	{
		reportWarning("ICP registration failed.");
		return;
	}

	Transform3D linearTransform = vesselReg.getLinearResult();
	std::cout << "v2v linear result:\n" << linearTransform << std::endl;
	//std::cout << "v2v inverted linear result:\n" << linearTransform.inverse() << std::endl;

	vesselReg.checkQuality(linearTransform);

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	Transform3D delta = linearTransform.inv();
	std::cout << "delta:\n" << delta << std::endl;
	//mServices.registrationService->applyImage2ImageRegistration(delta, "Vessel based");
	emit registrationChanged(delta);
	*/
}

/**Utililty class for debugging the SeansVesselRegistration class interactively.
 *
 * \sa PCICPWidget
 */
/*
class SeansVesselRegistrationDebugger
{
public:
	SeansVesselRegistrationDebugger(VisServicesPtr services, DataPtr movingData, DataPtr fixedData, double ltsRatio, bool linear) :
		mServices(services)
	{
		mRegistrator.mt_doOnlyLinear = linear;
		mRegistrator.mt_ltsRatio = ltsRatio;
		mRegistrator.mt_auto_lts = false;

		mContext = mRegistrator.createContext(movingData, fixedData);

		//mMovingData = mRegistrator.convertToPolyData(mContext->mSourcePoints);
		mMovingData = mContext->getMovingPoints();
		mFixedData = mContext->getFixedPoints();

		MeshPtr moving(new Mesh("v2vreg_moving", "v2vreg_moving", mMovingData));
		moving->setColor(QColor("red"));

		MeshPtr fixed(new Mesh("v2vreg_fixed", "v2vreg_fixed", mFixedData));
		fixed->setColor(QColor("green"));

		mPolyLines = vtkPolyDataPtr::New();
		MeshPtr lines(new Mesh("v2vreg_lines", "v2vreg_lines", mPolyLines));
		lines->setColor(QColor("cornflowerblue"));

		ViewPtr view = mServices->visualizationService->get3DView();

		m_mRep = GeometricRep::New();
		m_mRep->setMesh(moving);
		view->addRep(m_mRep);

		m_fRep = GeometricRep::New();
		m_fRep->setMesh(fixed);
		view->addRep(m_fRep);

		m_lineRep = GeometricRep::New();
		m_lineRep->setMesh(lines);
		view->addRep(m_lineRep);

		this->update();

		report("Initialized V2V algorithm (debug). Use Step to iterate.");
	}
	~SeansVesselRegistrationDebugger()
	{
		ViewPtr view = mServices->visualizationService->get3DView();
		view->removeRep(m_mRep);
		view->removeRep(m_fRep);
		view->removeRep(m_lineRep);
		report("Closed V2V algorithm (debug).");
	}
	void stepL()
	{
		if (!mContext)
			return;
		mRegistrator.performOneRegistration(mContext, true);
		this->update();
		report(QString("One Linear V2V iteration, metric=%1").arg(mContext->mMetric));
	}
	void stepNL()
	{
		if (!mContext)
			return;
		mRegistrator.performOneRegistration(mContext, false);
		this->update();
		report(QString("One Nonlinear V2V iteration, metric=%1").arg(mContext->mMetric));
	}
	void apply()
	{
//		if (!mContext)
//			return;

//		Transform3D linearTransform = mRegistrator.getLinearResult(mContext);
//		std::cout << "v2v linear result:\n" << linearTransform << std::endl;

//		mRegistrator.checkQuality(linearTransform);
//		Transform3D delta = linearTransform.inv();
//		mServices.registrationService->applyImage2ImageRegistration(delta, "Vessel based");

//		report(QString("Applied linear registration from debug iteration."));
	}
	void update()
	{
		if (!mContext)
			return;
		mRegistrator.computeDistances(mContext);

		vtkPolyDataPtr moving = mContext->getMovingPoints();
		mMovingData->SetPoints(moving->GetPoints());
		mMovingData->SetVerts(moving->GetVerts());

		vtkPolyDataPtr fixed = mContext->getFixedPoints();
		mFixedData->SetPoints(fixed->GetPoints());
		mFixedData->SetVerts(fixed->GetVerts());

		// draw lines
		mPolyLines->Allocate();
		vtkPointsPtr verts = vtkPointsPtr::New();
		for (int i = 0; i < mContext->mSortedSourcePoints->GetNumberOfPoints(); ++i)
		{
			verts->InsertNextPoint(mContext->mSortedSourcePoints->GetPoint(i));
			verts->InsertNextPoint(mContext->mSortedTargetPoints->GetPoint(i));

			vtkIdType connectivity[2];
			connectivity[0] = 2 * i;
			connectivity[1] = 2 * i + 1;
			mPolyLines->InsertNextCell(VTK_LINE, 2, connectivity);
		}
		mPolyLines->SetPoints(verts);
	}

private:
	SeansVesselReg mRegistrator;
	SeansVesselReg::ContextPtr mContext;
	vtkPolyDataPtr mMovingData, mFixedData;
	vtkPolyDataPtr mPolyLines;
	GeometricRepPtr m_mRep, m_fRep, m_lineRep;
	//	std::vector<GraphicalLine3DPtr> mLines;
	VisServicesPtr mServices;
};

void PCICPWidget::debugInit()
{
	mDebugger.reset(new SeansVesselRegistrationDebugger(mServices, mMoving, mFixed, mLTSRatioSpinBox->value(),
		mLinearCheckBox->isChecked()));
}
void PCICPWidget::debugRunOneLinearStep()
{
	if (mDebugger)
		mDebugger->stepL();
}
void PCICPWidget::debugRunOneNonlinearStep()
{
	if (mDebugger)
		mDebugger->stepNL();
}

void PCICPWidget::debugApply()
{
	if (mDebugger)
		mDebugger->apply();
}

void PCICPWidget::debugClear()
{
	mDebugger.reset();
}
*/
QWidget* PCICPWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);

	mLTSRatioSpinBox->setSingleStep(1);
	mLTSRatioSpinBox->setValue(80);

	mLinearCheckBox->setChecked(true);
	mAutoLTSCheckBox->setChecked(true);

	int line = 0;
	layout->addWidget(new QLabel("Auto LTS:"), line, 0);
	layout->addWidget(mAutoLTSCheckBox, line, 1);
	++line;
	layout->addWidget(new QLabel("LTS Ratio:"), line, 0);
	layout->addWidget(mLTSRatioSpinBox, line, 1);
	++line;
	layout->addWidget(new QLabel("Linear:"), line, 0);
	layout->addWidget(mLinearCheckBox, line, 1);
	++line;
	layout->addWidget(new QLabel("Debug"), line, 0);
	QHBoxLayout* debugLayout = new QHBoxLayout;
	layout->addLayout(debugLayout, line, 1, 1, 1);
	this->createAction(this, QIcon(), "Init",
		"Initialize the V2V algorithm.\n Display only, registration will not be updated in CustusX (Debug)",
		SLOT(debugInit()), debugLayout);
	this->createAction(this, QIcon(), "Lin", "Run one Linear step in the V2V algorithm. (Debug)",
		SLOT(debugRunOneLinearStep()), debugLayout);
	this->createAction(this, QIcon(), "NL",
		"Run one Nonlinear step in the V2V algorithm. (Should be one at the end only)(Debug)",
		SLOT(debugRunOneNonlinearStep()), debugLayout);
	this->createAction(this, QIcon(), "Apply", "Apply results from the debug iteration", SLOT(debugApply()), debugLayout);
	this->createAction(this, QIcon(), "Clear", "Clear debugging of the V2V algorithm.", SLOT(debugClear()), debugLayout);

	return retval;
}

}//namespace cx
