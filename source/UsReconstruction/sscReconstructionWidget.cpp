/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructionWidget.h"
#include "sscReconstructOutputValueParamsInterfaces.h"
#include "sscMessageManager.h"
#include "sscHelperWidgets.h"
#include "sscTypeConversions.h"

namespace ssc 
{

ReconstructionWidget::ReconstructionWidget(QWidget* parent, ReconstructerPtr reconstructer):
  QWidget(parent),
  mReconstructer(reconstructer)
{
  this->setWindowTitle("US Reconstruction");

  connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SLOT(paramsChangedSlot()));
  connect(mReconstructer.get(), SIGNAL(inputDataSelected(QString)), this, SLOT(inputDataSelected(QString)));

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
  mMaxVolSizeWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterMaxUSVolumeSize(mReconstructer)), outputVolGridLayout, 0);
  mSpacingWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterSpacing(mReconstructer)), outputVolGridLayout, 1);
  mDimXWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterXDim(mReconstructer)));
  mDimYWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterYDim(mReconstructer)));
  mDimZWidget = new ssc::SpinBoxGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterZDim(mReconstructer)));
  QHBoxLayout* outputVolDimLayout = new QHBoxLayout;
  outputVolDimLayout->addWidget(mDimXWidget);
  outputVolDimLayout->addWidget(mDimYWidget);
  outputVolDimLayout->addWidget(mDimZWidget);

  ssc::LabeledComboBoxWidget* orientationWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->mOrientationAdapter);
  ssc::LabeledComboBoxWidget* presetTFWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->mPresetTFAdapter);
  
  QWidget* reduceWidget = ssc::createDataWidget(this, mReconstructer->mMaskReduce);
  
  ssc::LabeledComboBoxWidget* algorithmWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->mAlgorithmAdapter);

  QWidget* alignTimestampsWidget = ssc::createDataWidget(this, mReconstructer->mAlignTimestamps);
  QWidget* timeCalibrationWidget = ssc::createDataWidget(this, mReconstructer->mTimeCalibration);
  QWidget* angioWidget = ssc::createDataWidget(this, mReconstructer->mAngioAdapter);


  mAlgorithmGroup = new QGroupBox("Algorithm", this);
  mAlgoLayout = new QGridLayout(mAlgorithmGroup);
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

void ReconstructionWidget::repopulateAlgorithmGroup()
{
	QString algoName = mReconstructer->mAlgorithmAdapter->getValue();

	if (algoName==mAlgorithmGroup->title())
		return;

	mAlgorithmGroup->setTitle(algoName);

	while (mAlgoLayout->count())
	{
		QWidget* child = mAlgoLayout->itemAt(0)->widget();
		if (!child)
			break; // ups : found layout inside, bail out
		child->setVisible(false);
		mAlgoLayout->removeWidget(child);
		//delete child;
	}
	 mAlgoLayout->update();

	 // delete widget objects (might be different than the widgets removed from the layout
	 for (unsigned i=0; i<mAlgoWidgets.size(); ++i)
		 delete mAlgoWidgets[i];
	 mAlgoWidgets.clear();

	std::vector<DataAdapterPtr> algoOption = mReconstructer->mAlgoOptions;
	for (unsigned i=0; i<algoOption.size(); ++i)
	{
	  QWidget* widget = ssc::createDataWidget(this, algoOption[i], mAlgoLayout, i);
	  mAlgoWidgets.push_back(widget);
	}
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
  if(mReconstructer->getSelectedData().isEmpty())
  {
    return;
  }

  mFileSelectWidget->setFilename(mhdFileName);
}


void ReconstructionWidget::selectData(QString filename)
{
  if(filename.isEmpty())
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
	repopulateAlgorithmGroup();

    ssc::Vector3D range = mReconstructer->getOutputVolumeParams().mExtent.range();

    QString extText = QString("%1,  %2,  %3").arg(range[0],0,'f',1).arg(range[1],0,'f',1).arg(range[2],0,'f',1);
    mExtentLineEdit->setText(extText);

    mInputSpacingLineEdit->setText(QString("%1").arg(mReconstructer->getOutputVolumeParams().mInputSpacing,0,'f',4));
}
  
}//namespace
