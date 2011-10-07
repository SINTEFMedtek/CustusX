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

namespace ssc
{

ReconstructionWidget::ReconstructionWidget(QWidget* parent, ReconstructManagerPtr reconstructer) :
	QWidget(parent), mReconstructer(reconstructer)
{
	this->setWindowTitle("US Reconstruction");

	connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SLOT(paramsChangedSlot()));
	connect(mReconstructer.get(), SIGNAL(inputDataSelected(QString)), this, SLOT(inputDataSelected(QString)));
	connect(mReconstructer.get(), SIGNAL(algorithmChanged()), this, SLOT(repopulateAlgorithmGroup()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	mFileSelectWidget = new FileSelectWidget(this);
	connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));

	QHBoxLayout* extentLayout = new QHBoxLayout;
	mExtentLineEdit = new QLineEdit(this);
	mExtentLineEdit->setReadOnly(true);
	extentLayout->addWidget(new QLabel("Extent(mm)", this));
	extentLayout->addWidget(mExtentLineEdit);

	QHBoxLayout* inputSpacingLayout = new QHBoxLayout;
	mInputSpacingLineEdit = new QLineEdit(this);
	mInputSpacingLineEdit->setReadOnly(true);
	inputSpacingLayout->addWidget(new QLabel("Input Spacing(mm)", this));
	inputSpacingLayout->addWidget(mInputSpacingLineEdit);

	mReconstructButton = new QPushButton("Reconstruct", this);
	connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

	QGroupBox* outputVolGroup = new QGroupBox("Output Volume", this);
	QVBoxLayout* outputVolLayout = new QVBoxLayout(outputVolGroup);

	QGridLayout* outputVolGridLayout = new QGridLayout;
	mMaxVolSizeWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(
		new DoubleDataAdapterMaxUSVolumeSize(mReconstructer)), outputVolGridLayout, 0);
	mSpacingWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterSpacing(
		mReconstructer)), outputVolGridLayout, 1);
	mDimXWidget = new ssc::SpinBoxGroupWidget(this,
		ssc::DoubleDataAdapterPtr(new DoubleDataAdapterXDim(mReconstructer)));
	mDimYWidget = new ssc::SpinBoxGroupWidget(this,
		ssc::DoubleDataAdapterPtr(new DoubleDataAdapterYDim(mReconstructer)));
	mDimZWidget = new ssc::SpinBoxGroupWidget(this,
		ssc::DoubleDataAdapterPtr(new DoubleDataAdapterZDim(mReconstructer)));
	QHBoxLayout* outputVolDimLayout = new QHBoxLayout;
	outputVolDimLayout->addWidget(mDimXWidget);
	outputVolDimLayout->addWidget(mDimYWidget);
	outputVolDimLayout->addWidget(mDimZWidget);

	ssc::LabeledComboBoxWidget* orientationWidget = new ssc::LabeledComboBoxWidget(this,
		mReconstructer->getParams()->mOrientationAdapter);
	ssc::LabeledComboBoxWidget* presetTFWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->getParams()->mPresetTFAdapter);

	QWidget* reduceWidget = ssc::createDataWidget(this, mReconstructer->getParams()->mMaskReduce);

	ssc::LabeledComboBoxWidget* algorithmWidget = new ssc::LabeledComboBoxWidget(this,
		mReconstructer->getParams()->mAlgorithmAdapter);

	QWidget* alignTimestampsWidget = ssc::createDataWidget(this, mReconstructer->getParams()->mAlignTimestamps);
	QWidget* timeCalibrationWidget = ssc::createDataWidget(this, mReconstructer->getParams()->mTimeCalibration);
	QWidget* angioWidget = ssc::createDataWidget(this, mReconstructer->getParams()->mAngioAdapter);

	mAlgorithmGroup = new QGroupBox("Algorithm", this);
	mAlgoLayout = new QStackedLayout(mAlgorithmGroup);
	this->repopulateAlgorithmGroup();

	topLayout->addWidget(mFileSelectWidget);
	topLayout->addLayout(inputSpacingLayout);
	topLayout->addWidget(angioWidget);
	topLayout->addWidget(outputVolGroup);
	outputVolLayout->addLayout(extentLayout);
	outputVolLayout->addLayout(outputVolGridLayout);
	outputVolLayout->addLayout(outputVolDimLayout);
	outputVolLayout->addWidget(orientationWidget);
	outputVolLayout->addWidget(presetTFWidget);
	outputVolLayout->addWidget(reduceWidget);
	outputVolLayout->addWidget(alignTimestampsWidget);
	outputVolLayout->addWidget(timeCalibrationWidget);
	topLayout->addWidget(algorithmWidget);
	topLayout->addWidget(mAlgorithmGroup);
	topLayout->addWidget(mReconstructButton);
	topLayout->addStretch();
}

/** Add the widgets for the current algorithm to a stacked widget.
 *  When algo is changed, even back to a previous algo, just hide the old
 *  and add a new on the stack.
 *
 */
void ReconstructionWidget::repopulateAlgorithmGroup()
{
	QString algoName = mReconstructer->getParams()->mAlgorithmAdapter->getValue();

	if (algoName == mAlgorithmGroup->title())
		return;

	mAlgorithmGroup->setTitle(algoName);

//	// look for an existing layout in the stack:
//	for (int i = 0; i < mAlgoLayout->count(); ++i)
//	{
//		QWidget* current = mAlgoLayout->widget(i);
//		if (current->objectName() == algoName)
//		{
//			mAlgoLayout->setCurrentIndex(i);
//			return;
//		}
//	}

	// No existing found,
	//  create a new stack element for this algo:
	QWidget* oneAlgoWidget = new QWidget(this);
	oneAlgoWidget->setObjectName(algoName+qstring_cast(mAlgoLayout->count()));
	mAlgoLayout->addWidget(oneAlgoWidget);
	QGridLayout* oneAlgoLayout = new QGridLayout(oneAlgoWidget);

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
	mReconstructer->reconstruct();
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
