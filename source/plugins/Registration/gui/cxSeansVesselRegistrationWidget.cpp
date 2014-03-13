#include "cxSeansVesselRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include "cxTypeConversions.h"
#include "cxMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxTimedAlgorithm.h"
#include "cxPatientData.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxRegistrationDataAdapters.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMesh.h"
#include "cxViewManager.h"
#include "cxView.h"
#include "cxGeometricRep.h"
#include <vtkCellArray.h>
#include "cxGraphicalPrimitives.h"

namespace cx
{

SeansVesselRegistrationWidget::SeansVesselRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent) :
	RegistrationBaseWidget(regManager, parent, "SeansVesselRegistrationWidget", "Seans Vessel Registration"),
		mLTSRatioSpinBox(new QSpinBox()), mLinearCheckBox(new QCheckBox()), mAutoLTSCheckBox(new QCheckBox()),
		mRegisterButton(new QPushButton("Register"))
{
	mRegisterButton->setEnabled(false);
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	connect(mManager.get(), SIGNAL(fixedDataChanged(QString)), this, SLOT(inputChanged()));
	connect(mManager.get(), SIGNAL(movingDataChanged(QString)), this, SLOT(inputChanged()));

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

	mFixedImage.reset(new RegistrationFixedImageStringDataAdapter(regManager));
	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new RegistrationMovingImageStringDataAdapter(regManager));
	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	layout->addLayout(entryLayout, 0, 0, 2, 2);
	layout->addWidget(mRegisterButton, 2, 0);
	layout->addWidget(mVesselRegOptionsButton, 2, 1);
	layout->addWidget(mVesselRegOptionsWidget, 3, 0, 1, 2);
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

void SeansVesselRegistrationWidget::inputChanged()
{
	if(mManager->getMovingData() && mManager->getFixedData())
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

	SeansVesselReg vesselReg;
	vesselReg.mt_auto_lts = true;
	vesselReg.mt_ltsRatio = mLTSRatioSpinBox->value();
	vesselReg.mt_doOnlyLinear = mLinearCheckBox->isChecked();
	vesselReg.mt_auto_lts = mAutoLTSCheckBox->isChecked();

	if (vesselReg.mt_auto_lts)
	{
		messageManager()->sendDebug("Using automatic lts_ratio");
	}
	else
	{
		messageManager()->sendDebug("Using lts_ratio: " + qstring_cast(vesselReg.mt_ltsRatio));
	}

	if(!mManager->getMovingData())
	{
		messageManager()->sendWarning("Moving volume not set.");
		return;
	}
	else if(!mManager->getFixedData())
	{
		messageManager()->sendWarning("Fixed volume not set.");
		return;
	}

	bool success = vesselReg.execute(mManager->getMovingData(), mManager->getFixedData(), logPath);
	if (!success)
	{
		messageManager()->sendWarning("Vessel registration failed.");
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

		View* view = viewManager()->get3DView();

		m_mRep = GeometricRep::New(moving->getUid(), moving->getName());
		m_mRep->setMesh(moving);
		view->addRep(m_mRep);

		m_fRep = GeometricRep::New(fixed->getUid(), fixed->getName());
		m_fRep->setMesh(fixed);
		view->addRep(m_fRep);

		m_lineRep = GeometricRep::New(lines->getUid(), lines->getName());
		m_lineRep->setMesh(lines);
		view->addRep(m_lineRep);

		this->update();

		messageManager()->sendInfo("Initialized V2V algorithm (debug). Use Step to iterate.");
	}
	~SeansVesselRegistrationDebugger()
	{
		View* view = viewManager()->get3DView();
		view->removeRep(m_mRep);
		view->removeRep(m_fRep);
		view->removeRep(m_lineRep);
		messageManager()->sendInfo("Closed V2V algorithm (debug).");
	}
	void stepL()
	{
		if (!mContext)
			return;
		mRegistrator.performOneRegistration(mContext, true);
		this->update();
		messageManager()->sendInfo(QString("One Linear V2V iteration, metric=%1").arg(mContext->mMetric));
	}
	void stepNL()
	{
		if (!mContext)
			return;
		mRegistrator.performOneRegistration(mContext, false);
		this->update();
		messageManager()->sendInfo(QString("One Nonlinear V2V iteration, metric=%1").arg(mContext->mMetric));
	}
	void apply()
	{
		if (!mContext)
			return;

		Transform3D linearTransform = mRegistrator.getLinearResult(mContext);
		std::cout << "v2v linear result:\n" << linearTransform << std::endl;

		mRegistrator.checkQuality(linearTransform);
		Transform3D delta = linearTransform.inv();
		mManager->applyImage2ImageRegistration(delta, "Vessel based");

		messageManager()->sendInfo(QString("Applied linear registration from debug iteration."));
	}
	void update()
	{
		if (!mContext)
			return;
		mRegistrator.computeDistances(mContext);
//		vtkPolyDataPtr temp = mRegistrator.convertToPolyData(mContext->mSourcePoints);
//		mMovingData->SetPoints(temp->GetPoints());
//		mMovingData->SetVerts(temp->GetVerts());

		vtkPolyDataPtr moving = mContext->getMovingPoints();
		mMovingData->SetPoints(moving->GetPoints());
		mMovingData->SetVerts(moving->GetVerts());

		vtkPolyDataPtr fixed = mContext->getFixedPoints();
		mFixedData->SetPoints(fixed->GetPoints());
		mFixedData->SetVerts(fixed->GetVerts());

		//		// draw lines: slow but nice
		//		mLines.clear();
		//		View* view = viewManager()->get3DView();
		//		for (int i=0; i<mContext->mSortedSourcePoints->GetNumberOfPoints(); ++i)
		//		{
		//			GraphicalLine3DPtr line(new GraphicalLine3D(view->getRenderer()));
		//			line->setValue(Vector3D(mContext->mSortedSourcePoints->GetPoint(i)),
		//				Vector3D(mContext->mSortedTargetPoints->GetPoint(i)));
		//			line->setColor(Vector3D(1,0,0));
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
	SeansVesselReg mRegistrator;
	SeansVesselReg::ContextPtr mContext;
	RegistrationManagerPtr mManager;
	vtkPolyDataPtr mMovingData, mFixedData;
	vtkPolyDataPtr mPolyLines;
	GeometricRepPtr m_mRep, m_fRep, m_lineRep;
	//	std::vector<GraphicalLine3DPtr> mLines;
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
