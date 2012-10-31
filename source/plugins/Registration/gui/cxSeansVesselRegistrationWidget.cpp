#include "cxSeansVesselRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxTimedAlgorithm.h"
#include "cxPatientData.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxRegistrationDataAdapters.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "sscMesh.h"
#include "cxViewManager.h"
#include "cxView3D.h"
#include "sscGeometricRep.h"
#include <vtkCellArray.h>
#include "sscGraphicalPrimitives.h"

namespace cx
{

SeansVesselRegistrationWidget::SeansVesselRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
	RegistrationBaseWidget(regManager, parent, "SeansVesselRegistrationWidget", "Seans Vessel Registration"),
		mLTSRatioSpinBox(new QSpinBox()), mLinearCheckBox(new QCheckBox()), mAutoLTSCheckBox(new QCheckBox()),
		mRegisterButton(new QPushButton("Register"))
{

	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	QGridLayout* layout = new QGridLayout();
	topLayout->addLayout(layout);

	QPushButton* vesselRegOptionsButton = new QPushButton("Options", this);
	vesselRegOptionsButton->setCheckable(true);

	QGroupBox* vesselRegOptionsWidget = this->createGroupbox(this->createOptionsWidget(),
		"Vessel registration options");
	connect(vesselRegOptionsButton, SIGNAL(clicked(bool)), vesselRegOptionsWidget, SLOT(setVisible(bool)));
	vesselRegOptionsWidget->setVisible(vesselRegOptionsButton->isChecked());

	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new RegistrationFixedImageStringDataAdapter(regManager));
	new ssc::LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new RegistrationMovingImageStringDataAdapter(regManager));
	new ssc::LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	layout->addLayout(entryLayout, 0, 0, 2, 2);
	layout->addWidget(mRegisterButton, 2, 0);
	layout->addWidget(vesselRegOptionsButton, 2, 1);
	layout->addWidget(vesselRegOptionsWidget, 3, 0, 1, 2);
}

SeansVesselRegistrationWidget::~SeansVesselRegistrationWidget()
{
}

QString SeansVesselRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Seans Vessel Registration.</h3>"
		"<p>Select two datasets you want to registere to eachother, adjust the input parameters.</p>"
		"<p><i>Adjust the parameters and click the register button.</i></p>"
		"</html>";
}

void SeansVesselRegistrationWidget::registerSlot()
{
//	int lts_ratio = mLTSRatioSpinBox->value();
//	double stop_delta = 0.001; //TODO, add user interface
//	double lambda = 0; //TODO, add user interface
//	double sigma = 1.0; //TODO, add user interface
//	bool lin_flag = mLinearCheckBox->isChecked(); //TODO, add user interface
//	int sample = 1; //TODO, add user interface
//	int single_point_thre = 1; //TODO, add user interface
//	bool verbose = 1; //TODO, add user interface

	QString logPath = patientService()->getPatientData()->getActivePatientFolder() + "/Logs/";

//	mManager->doVesselRegistration(lts_ratio, stop_delta, lambda, sigma, lin_flag, sample, single_point_thre, verbose,
//		logPath);

	ssc::SeansVesselReg vesselReg;
	vesselReg.mt_auto_lts = true;
	vesselReg.mt_ltsRatio = mLTSRatioSpinBox->value();
	vesselReg.mt_doOnlyLinear = mLinearCheckBox->isChecked();
	vesselReg.mt_auto_lts = mAutoLTSCheckBox->isChecked();

	if (vesselReg.mt_auto_lts)
		ssc::messageManager()->sendDebug("Using automatic lts_ratio");
	else
		ssc::messageManager()->sendDebug("Using lts_ratio: " + qstring_cast(vesselReg.mt_ltsRatio));

	bool success = vesselReg.execute(mManager->getMovingData(), mManager->getFixedData(), logPath);
	if (!success)
	{
		ssc::messageManager()->sendWarning("Vessel registration failed.");
		return;
	}

	ssc::Transform3D linearTransform = vesselReg.getLinearResult();
	std::cout << "v2v linear result:\n" << linearTransform << std::endl;
	//std::cout << "v2v inverted linear result:\n" << linearTransform.inverse() << std::endl;

	vesselReg.checkQuality(linearTransform);

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	ssc::Transform3D delta = linearTransform.inv();
	//std::cout << "delta:\n" << delta << std::endl;
	mManager->applyImage2ImageRegistration(delta, "Vessel based");
}

/**Utililty class for debugging the SeansVesselRegistration class interactively.
 *
 * \sa SeansVesselRegistrationWidget
 */
class SeansVesselRegistrationDebugger
{
public:
	SeansVesselRegistrationDebugger(RegistrationManagerPtr manager, double ltsRatio, bool linear)
	{
		mRegistrator.mt_doOnlyLinear = linear;
		mRegistrator.mt_ltsRatio = ltsRatio;
		mRegistrator.mt_auto_lts = false;

		mManager = manager;
		mContext = mRegistrator.createContext(mManager->getMovingData(), mManager->getFixedData());

		mMovingData = mRegistrator.convertToPolyData(mContext->mSourcePoints);

		ssc::MeshPtr moving(new ssc::Mesh("v2vreg_moving", "v2vreg_moving", mMovingData));
		moving->setColor(QColor("red"));

		ssc::MeshPtr fixed(new ssc::Mesh("v2vreg_fixed", "v2vreg_fixed", mContext->mTargetPoints));
		fixed->setColor(QColor("green"));

		mPolyLines = vtkPolyDataPtr::New();
		ssc::MeshPtr lines(new ssc::Mesh("v2vreg_lines", "v2vreg_lines", mPolyLines));
		lines->setColor(QColor("cornflowerblue"));

		ssc::View* view = viewManager()->get3DView();

		m_mRep = ssc::GeometricRep::New(moving->getUid(), moving->getName());
		m_mRep->setMesh(moving);
		view->addRep(m_mRep);

		m_fRep = ssc::GeometricRep::New(fixed->getUid(), fixed->getName());
		m_fRep->setMesh(fixed);
		view->addRep(m_fRep);

		m_lineRep = ssc::GeometricRep::New(lines->getUid(), lines->getName());
		m_lineRep->setMesh(lines);
		view->addRep(m_lineRep);

		this->update();

		ssc::messageManager()->sendInfo("Initialized V2V algorithm (debug). Use Step to iterate.");
	}
	~SeansVesselRegistrationDebugger()
	{
		ssc::View* view = viewManager()->get3DView();
		view->removeRep(m_mRep);
		view->removeRep(m_fRep);
		view->removeRep(m_lineRep);
		ssc::messageManager()->sendInfo("Closed V2V algorithm (debug).");
	}
	void stepL()
	{
		if (!mContext)
			return;
		mRegistrator.performOneRegistration(mContext, true);
		this->update();
		ssc::messageManager()->sendInfo(QString("One Linear V2V iteration, metric=%1").arg(mContext->mMetric));
	}
	void stepNL()
	{
		if (!mContext)
			return;
		mRegistrator.performOneRegistration(mContext, false);
		this->update();
		ssc::messageManager()->sendInfo(QString("One Nonlinear V2V iteration, metric=%1").arg(mContext->mMetric));
	}
	void apply()
	{
		if (!mContext)
			return;

		ssc::Transform3D linearTransform = mRegistrator.getLinearResult(mContext);
		std::cout << "v2v linear result:\n" << linearTransform << std::endl;

		mRegistrator.checkQuality(linearTransform);
		ssc::Transform3D delta = linearTransform.inv();
		mManager->applyImage2ImageRegistration(delta, "Vessel based");

		ssc::messageManager()->sendInfo(QString("Applied linear registration from debug iteration."));
	}
	void update()
	{
		if (!mContext)
			return;
		mRegistrator.computeDistances(mContext);
		vtkPolyDataPtr temp = mRegistrator.convertToPolyData(mContext->mSourcePoints);
		mMovingData->SetPoints(temp->GetPoints());
		mMovingData->SetVerts(temp->GetVerts());

		//		// draw lines: slow but nice
		//		mLines.clear();
		//		ssc::View* view = viewManager()->get3DView();
		//		for (int i=0; i<mContext->mSortedSourcePoints->GetNumberOfPoints(); ++i)
		//		{
		//			ssc::GraphicalLine3DPtr line(new ssc::GraphicalLine3D(view->getRenderer()));
		//			line->setValue(ssc::Vector3D(mContext->mSortedSourcePoints->GetPoint(i)),
		//				ssc::Vector3D(mContext->mSortedTargetPoints->GetPoint(i)));
		//			line->setColor(ssc::Vector3D(1,0,0));
		//			mLines.push_back(line);
		//		}

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
	ssc::SeansVesselReg mRegistrator;
	ssc::SeansVesselReg::ContextPtr mContext;
	RegistrationManagerPtr mManager;
	vtkPolyDataPtr mMovingData;
	vtkPolyDataPtr mPolyLines;
	ssc::GeometricRepPtr m_mRep, m_fRep, m_lineRep;
	//	std::vector<ssc::GraphicalLine3DPtr> mLines;
};

void SeansVesselRegistrationWidget::debugInit()
{
	mDebugger.reset(new SeansVesselRegistrationDebugger(mManager, mLTSRatioSpinBox->value(),
		mLinearCheckBox->isChecked()));
}
void SeansVesselRegistrationWidget::debugRunOneLinearStep()
{
	if (mDebugger)
		mDebugger->stepL();
}
void SeansVesselRegistrationWidget::debugRunOneNonlinearStep()
{
	if (mDebugger)
		mDebugger->stepNL();
}

void SeansVesselRegistrationWidget::debugApply()
{
	if (mDebugger)
		mDebugger->apply();
}

void SeansVesselRegistrationWidget::debugClear()
{
	mDebugger.reset();
}

QWidget* SeansVesselRegistrationWidget::createOptionsWidget()
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
