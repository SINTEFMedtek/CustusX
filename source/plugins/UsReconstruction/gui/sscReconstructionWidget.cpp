/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructionWidget.h"
#include "sscReconstructOutputValueParamsInterfaces.h"
#include "sscMessageManager.h"
#include "sscHelperWidgets.h"
#include "sscTypeConversions.h"
#include "sscReconstructer.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxSettings.h"

namespace cx
{

ReconstructionWidget::ReconstructionWidget(QWidget* parent, ssc::ReconstructManagerPtr reconstructer) :
	BaseWidget(parent, "", "US Reconstruction"), mReconstructer(reconstructer)
{
	mThreadedTimedReconstructer.reset(new ssc::ThreadedTimedReconstructer(mReconstructer));
	connect(mThreadedTimedReconstructer.get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));

	connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SLOT(paramsChangedSlot()));
	connect(mReconstructer.get(), SIGNAL(inputDataSelected(QString)), this, SLOT(inputDataSelected(QString)));
	connect(mReconstructer.get(), SIGNAL(algorithmChanged()), this, SLOT(repopulateAlgorithmGroup()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	mFileSelectWidget = new ssc::FileSelectWidget(this);
	connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));

	QHBoxLayout* extentLayout = new QHBoxLayout;
	mExtentLineEdit = new QLineEdit(this);
	mExtentLineEdit->setReadOnly(true);
	extentLayout->addWidget(new QLabel("Extent", this));
	extentLayout->addWidget(mExtentLineEdit);

//	QHBoxLayout* inputSpacingLayout = new QHBoxLayout;
	mInputSpacingLineEdit = new QLineEdit(this);
	mInputSpacingLineEdit->setReadOnly(true);
	QLabel* inputSpacingLabel = new QLabel("Spacing In", this);

	mReconstructButton = new QPushButton("Reconstruct", this);
	connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->attach(mThreadedTimedReconstructer);

	QGridLayout* sizesLayout = new QGridLayout;
	mMaxVolSizeWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new ssc::DoubleDataAdapterMaxUSVolumeSize(mReconstructer)));
	sizesLayout->addWidget(inputSpacingLabel, 0, 0);
	sizesLayout->addWidget(mInputSpacingLineEdit, 0, 1);
	sizesLayout->addLayout(extentLayout, 0, 2);
	mSpacingWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new ssc::DoubleDataAdapterSpacing(mReconstructer)), sizesLayout, 1);
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

	ssc::LabeledComboBoxWidget* presetTFWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->getParams()->mPresetTFAdapter);
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
	ssc::LabeledComboBoxWidget* algorithmWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->getParams()->mAlgorithmAdapter);
	algoOuterLayout->addWidget(algorithmWidget);
	mAlgoLayout = new QStackedLayout;
	this->repopulateAlgorithmGroup();
	algoOuterLayout->addLayout(mAlgoLayout);
	//algoOuterLayout->addStretch();
	layout->addWidget(mAlgorithmGroup, line, 0, 1, 2);
	++line;

	ssc::createDataWidget(this, mReconstructer->getParams()->mAlignTimestamps, layout, line++);
	ssc::createDataWidget(this, mReconstructer->getParams()->mAngioAdapter, layout, line++);
	ssc::createDataWidget(this, mReconstructer->getParams()->mTimeCalibration, layout, line++);
	ssc::createDataWidget(this, mReconstructer->getParams()->mMaskReduce, layout, line++);
	ssc::createDataWidget(this, mReconstructer->getParams()->mOrientationAdapter, layout, line++);

	mDimXWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new ssc::DoubleDataAdapterXDim(mReconstructer)));
	mDimYWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new ssc::DoubleDataAdapterYDim(mReconstructer)));
	mDimZWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new ssc::DoubleDataAdapterZDim(mReconstructer)));
	QHBoxLayout* outputVolDimLayout = new QHBoxLayout;
	outputVolDimLayout->addWidget(mDimXWidget);
	outputVolDimLayout->addWidget(mDimYWidget);
	outputVolDimLayout->addWidget(mDimZWidget);
	layout->addLayout(outputVolDimLayout, line, 0, 1, 2);

	return retval;
}

/** Add the widgets for the current algorithm to a stacked widget.
 *  When algo is changed, even back to a previous algo, just hide the old
 *  and add a new on the stack.
 *
 */
void ReconstructionWidget::repopulateAlgorithmGroup()
{
	QString algoName = mReconstructer->getParams()->mAlgorithmAdapter->getValue();

	if (mAlgoLayout->currentWidget() && (algoName == mAlgoLayout->currentWidget()->objectName()))
		return;

//	mAlgorithmGroup->setTitle("<removed>");
//	mAlgorithmGroup->setTitle(algoName);

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
		ssc::createDataWidget(oneAlgoWidget, algoOption[i], oneAlgoLayout, i);
	}

	mAlgoLayout->setCurrentWidget(oneAlgoWidget);
}

QString ReconstructionWidget::getCurrentPath()
{
	return QFileInfo(mReconstructer->getSelectedData()).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
	ssc::messageManager()->sendInfo("Reconstructing...");
	qApp->processEvents();

//	mThreadedTimedReconstructer.reset(new ssc::ThreadedTimedReconstructer(mReconstructer));
//	connect(mThreadedTimedReconstructer.get(), SIGNAL(finished()), this, SLOT(reconstructFinishedSlot()));
	mThreadedTimedReconstructer->start();
	mReconstructButton->setEnabled(false);
//	std::cout << "================reconstruct started" << std::endl;

//	mReconstructer->reconstruct();
}

void ReconstructionWidget::reconstructFinishedSlot()
{
	mReconstructButton->setEnabled(true);
}

void ReconstructionWidget::reload()
{
	this->selectData(mReconstructer->getSelectedData());
}

/**Called when data is loaded into reconstructer.
 *
 */
void ReconstructionWidget::inputDataSelected(QString mhdFileName)
{
	if (mReconstructer->getSelectedData().isEmpty())
	{
		return;
	}

	mFileSelectWidget->setFilename(mhdFileName);
}

void ReconstructionWidget::selectData(QString filename)
{
	if (filename.isEmpty())
	{
		ssc::messageManager()->sendWarning("no file selected");
		return;
	}

	mReconstructer->selectData(filename);
}

/** Called when parameters in the reconstructer has changed
 *
 */
void ReconstructionWidget::paramsChangedSlot()
{
//	repopulateAlgorithmGroup();

	ssc::Vector3D range = mReconstructer->getOutputVolumeParams().mExtent.range();

	QString extText =
		QString("%1,  %2,  %3").arg(range[0], 0, 'f', 1).arg(range[1], 0, 'f', 1).arg(range[2], 0, 'f', 1);
	mExtentLineEdit->setText(extText);

	mInputSpacingLineEdit->setText(QString("%1").arg(mReconstructer->getOutputVolumeParams().mInputSpacing, 0, 'f', 4));
}

}//namespace
