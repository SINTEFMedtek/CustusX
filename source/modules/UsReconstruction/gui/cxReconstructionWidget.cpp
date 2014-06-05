/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "cxReconstructionWidget.h"
#include "cxReconstructOutputValueParamsInterfaces.h"
#include "cxReporter.h"
#include "cxHelperWidgets.h"
#include "cxTypeConversions.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxSettings.h"
#include "cxReconstructParams.h"
#include "cxTimedAlgorithm.h"
#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"

namespace cx
{

ReconstructionWidget::ReconstructionWidget(QWidget* parent, ReconstructionManagerPtr reconstructer) :
	BaseWidget(parent, "", "US Reconstruction"), mReconstructer(reconstructer)
{
	connect(mReconstructer.get(), SIGNAL(reconstructAboutToStart()), this, SLOT(reconstructAboutToStartSlot()));
	connect(mReconstructer.get(), SIGNAL(reconstructStarted()), this, SLOT(reconstructStartedSlot()));
	connect(mReconstructer.get(), SIGNAL(reconstructFinished()), this, SLOT(reconstructFinishedSlot()));

	connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SLOT(paramsChangedSlot()));
	connect(mReconstructer.get(), SIGNAL(inputDataSelected(QString)), this, SLOT(inputDataSelected(QString)));
	connect(mReconstructer.get(), SIGNAL(algorithmChanged()), this, SLOT(repopulateAlgorithmGroup()));


	QVBoxLayout* topLayout = new QVBoxLayout(this);

	mFileSelectWidget = new FileSelectWidget(this);
	connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));
	mFileSelectWidget->setNameFilter(QStringList() << "*.fts");
	connect(mReconstructer.get(), SIGNAL(newInputDataAvailable(QString)), mFileSelectWidget, SLOT(refresh()));

	QHBoxLayout* extentLayout = new QHBoxLayout;
	mExtentLineEdit = new QLineEdit(this);
	mExtentLineEdit->setReadOnly(true);
	extentLayout->addWidget(new QLabel("Extent", this));
	extentLayout->addWidget(mExtentLineEdit);

	mInputSpacingLineEdit = new QLineEdit(this);
	mInputSpacingLineEdit->setReadOnly(true);
	QLabel* inputSpacingLabel = new QLabel("Spacing In", this);

	mReconstructButton = new QPushButton("Reconstruct", this);
	connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;

	QGridLayout* sizesLayout = new QGridLayout;
	mMaxVolSizeWidget = new SpinBoxGroupWidget(this, mReconstructer->getParams()->mMaxVolumeSize);
	sizesLayout->addWidget(inputSpacingLabel, 0, 0);
	sizesLayout->addWidget(mInputSpacingLineEdit, 0, 1);
	sizesLayout->addLayout(extentLayout, 0, 2);
	mSpacingWidget = new SpinBoxGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterSpacing(mReconstructer)), sizesLayout, 1);
	sizesLayout->addWidget(mMaxVolSizeWidget, 1, 2);

	QHBoxLayout* runLayout = new QHBoxLayout;
	topLayout->addLayout(runLayout);
	runLayout->addWidget(mReconstructButton);
	this->createAction(this,
	      QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
	      "Details", "Show Advanced Settings",
	      SLOT(toggleDetailsSlot()),
	      runLayout);

	topLayout->addWidget(mFileSelectWidget);
	topLayout->addLayout(sizesLayout);

	LabeledComboBoxWidget* presetTFWidget = new LabeledComboBoxWidget(this, mReconstructer->getParams()->mPresetTFAdapter);
	topLayout->addWidget(presetTFWidget);

	mOptionsWidget = this->createOptionsWidget();
	mOptionsWidget->setVisible(settings()->value("reconstruction/guiShowDetails").toBool());
	topLayout->addWidget(mOptionsWidget);

	topLayout->addStretch();
	topLayout->addWidget(mTimedAlgorithmProgressBar);
}

QString ReconstructionWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>US 3D Reconstruction.</h3>"
		"<p><i>Reconstruct 3D US data from acquired a 2D sequence.</i></br>"
		"</html>";
}

void ReconstructionWidget::toggleDetailsSlot()
{
  mOptionsWidget->setVisible(!mOptionsWidget->isVisible());
  settings()->setValue("reconstruction/guiShowDetails", mOptionsWidget->isVisible());
}

QWidget* ReconstructionWidget::createOptionsWidget()
{
	QWidget* retval = new QWidget(this);
	QGridLayout* layout = new QGridLayout(retval);
	layout->setMargin(0);

	int line = 0;

	layout->addWidget(this->createHorizontalLine(), line, 0, 1, 2);
	++line;

	mAlgorithmGroup = new QFrame(this);
	mAlgorithmGroup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	mAlgorithmGroup->setSizePolicy(mAlgorithmGroup->sizePolicy().horizontalPolicy(),QSizePolicy::Fixed);

	QVBoxLayout* algoOuterLayout = new QVBoxLayout(mAlgorithmGroup);
	LabeledComboBoxWidget* algorithmWidget = new LabeledComboBoxWidget(this, mReconstructer->getParams()->mAlgorithmAdapter);
	algoOuterLayout->addWidget(algorithmWidget);
	mAlgoLayout = new QStackedLayout;
	this->repopulateAlgorithmGroup();
	algoOuterLayout->addLayout(mAlgoLayout);
	//algoOuterLayout->addStretch();
	layout->addWidget(mAlgorithmGroup, line, 0, 1, 2);
	++line;

	sscCreateDataWidget(this, mReconstructer->getParams()->mAngioAdapter, layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParams()->mCreateBModeWhenAngio, layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParams()->mOrientationAdapter, layout, line++);
	layout->addWidget(this->createHorizontalLine(), line++, 0, 1, 2);

	mDimXWidget = new SpinBoxGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterXDim(mReconstructer)));
	mDimYWidget = new SpinBoxGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterYDim(mReconstructer)));
	mDimZWidget = new SpinBoxGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterZDim(mReconstructer)));
	QHBoxLayout* outputVolDimLayout = new QHBoxLayout;
	outputVolDimLayout->addWidget(mDimXWidget);
	outputVolDimLayout->addWidget(mDimYWidget);
	outputVolDimLayout->addWidget(mDimZWidget);
	layout->addLayout(outputVolDimLayout, line++, 0, 1, 2);

	sscCreateDataWidget(this, mReconstructer->getParams()->mAlignTimestamps, layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParams()->mTimeCalibration, layout, line++);
	sscCreateDataWidget(this, mReconstructer->getParams()->mMaskReduce, layout, line++);

	return retval;
}

void ReconstructionWidget::repopulateAlgorithmGroup()
{
	QString algoName = mReconstructer->getParams()->mAlgorithmAdapter->getValue();

	if (mAlgoLayout->currentWidget() && (algoName == mAlgoLayout->currentWidget()->objectName()))
		return;

	for (int i=0; i<mAlgoLayout->count(); ++i)
	{
		if (algoName==mAlgoLayout->widget(i)->objectName())
		{
			mAlgoLayout->setCurrentIndex(i);
			return;
		}
	}

	// No existing found,
	//  create a new stack element for this algo:
	QWidget* oneAlgoWidget = new QWidget(this);
	oneAlgoWidget->setObjectName(algoName);
	mAlgoLayout->addWidget(oneAlgoWidget);
	QGridLayout* oneAlgoLayout = new QGridLayout(oneAlgoWidget);
	oneAlgoLayout->setMargin(0);

	std::vector<DataAdapterPtr> algoOption = mReconstructer->getAlgoOptions();
	for (unsigned i = 0; i < algoOption.size(); ++i)
	{
		sscCreateDataWidget(oneAlgoWidget, algoOption[i], oneAlgoLayout, i);
	}

	mAlgoLayout->setCurrentWidget(oneAlgoWidget);
}

QString ReconstructionWidget::getCurrentPath()
{
	return QFileInfo(mReconstructer->getSelectedFilename()).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
	mReconstructer->startReconstruction();
}


void ReconstructionWidget::reload()
{
	this->selectData(mReconstructer->getSelectedFilename());
}

/**Called when data is loaded into reconstructer.
 *
 */
void ReconstructionWidget::inputDataSelected(QString mhdFileName)
{
	if (mReconstructer->getSelectedFilename().isEmpty())
	{
		return;
	}	

	mFileSelectWidget->setFilename(mhdFileName);
}

void ReconstructionWidget::selectData(QString filename)
{
	if (filename.isEmpty())
	{
		reportWarning("no file selected");
		return;
	}

	mReconstructer->selectData(filename);
}

/** Called when parameters in the reconstructer has changed
 *
 */
void ReconstructionWidget::paramsChangedSlot()
{
	Vector3D range = mReconstructer->getOutputVolumeParams().getExtent().range();

	QString extText =
		QString("%1,  %2,  %3").arg(range[0], 0, 'f', 1).arg(range[1], 0, 'f', 1).arg(range[2], 0, 'f', 1);
	mExtentLineEdit->setText(extText);

	mInputSpacingLineEdit->setText(QString("%1").arg(mReconstructer->getOutputVolumeParams().getInputSpacing(), 0, 'f', 4));
}

void ReconstructionWidget::reconstructAboutToStartSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mReconstructer->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->attach(threads);
}

void ReconstructionWidget::reconstructStartedSlot()
{
	mReconstructButton->setEnabled(false);
}

void ReconstructionWidget::reconstructFinishedSlot()
{
	std::set<cx::TimedAlgorithmPtr> threads = mReconstructer->getThreadedReconstruction();
	mTimedAlgorithmProgressBar->detach(threads);
		mReconstructButton->setEnabled(true);
}


}//namespace
