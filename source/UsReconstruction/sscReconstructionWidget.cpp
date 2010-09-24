/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */
#include "sscReconstructionWidget.h"
#include "sscReconstructOutputValueParamsInterfaces.h"
#include "sscMessageManager.h"
#include "sscHelperWidgets.h"

namespace ssc 
{


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


ReconstructionWidget::ReconstructionWidget(QWidget* parent, XmlOptionFile settings, QString shaderPath):
  QWidget(parent),
  mReconstructer(new Reconstructer(settings, shaderPath))
{
  this->setWindowTitle("US Reconstruction");

  connect(mReconstructer.get(), SIGNAL(paramsChanged()), this, SLOT(paramsChangedSlot()));

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  QHBoxLayout* dataLayout = new QHBoxLayout;
  mDataComboBox = new QComboBox(this);
  connect(mDataComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(currentDataComboIndexChanged(const QString&)));
  //this->updateComboBox();

  mSelectDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Select data"), this);
  connect(mSelectDataAction, SIGNAL(triggered()), this, SLOT(selectData()));
  mSelectDataButton = new QToolButton(this);
  mSelectDataButton->setDefaultAction(mSelectDataAction);

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

//  mReloadButton = new QPushButton("Reload", this);
//  connect(mReloadButton, SIGNAL(clicked()), this, SLOT(reload()));
  mReconstructButton = new QPushButton("Reconstruct", this);
  connect(mReconstructButton, SIGNAL(clicked()), this, SLOT(reconstruct()));

  QGroupBox* outputVolGroup = new QGroupBox("Output Volume", this);
  QVBoxLayout* outputVolLayout = new QVBoxLayout(outputVolGroup);

  QGridLayout* outputVolGridLayout = new QGridLayout;
  mMaxVolSizeWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterMaxUSVolumeSize(mReconstructer)), outputVolGridLayout, 0);
  mSpacingWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterSpacing(mReconstructer)), outputVolGridLayout, 1);
  mDimXWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterXDim(mReconstructer)), outputVolGridLayout, 2);
  mDimYWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterYDim(mReconstructer)), outputVolGridLayout, 3);
  mDimZWidget = new ssc::SliderGroupWidget(this, ssc::DoubleDataAdapterPtr(new DoubleDataAdapterZDim(mReconstructer)), outputVolGridLayout, 4);

  //ssc::StringDataAdapterPtr orientation = this->generateStringDataAdapter("Orientation");
  ssc::LabeledComboBoxWidget* orientationWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->mOrientationAdapter);
  
  QWidget* reduceWidget = ssc::createDataWidget(this, mReconstructer->mMaskReduce);
  
  //ssc::StringDataAdapterPtr algorithm = this->generateStringDataAdapter("Algorithm");
  ssc::LabeledComboBoxWidget* algorithmWidget = new ssc::LabeledComboBoxWidget(this, mReconstructer->mAlgorithmAdapter);

  mAlgorithmGroup = new QGroupBox("Algorithm", this);
  mAlgoLayout = new QGridLayout(mAlgorithmGroup);
  repopulateAlgorithmGroup();

  topLayout->addLayout(dataLayout);
    dataLayout->addWidget(mDataComboBox);
    dataLayout->addWidget(mSelectDataButton);
  //topLayout->addWidget(mReloadButton);
  topLayout->addWidget(outputVolGroup);
    outputVolLayout->addLayout(extentLayout);
    outputVolLayout->addLayout(inputSpacingLayout);
    outputVolLayout->addLayout(outputVolGridLayout);
    outputVolLayout->addWidget(orientationWidget);
    outputVolLayout->addWidget(reduceWidget);
  topLayout->addWidget(algorithmWidget);
  topLayout->addWidget(mAlgorithmGroup);
  topLayout->addWidget(mReconstructButton);
  topLayout->addStretch();

  //this->selectData(defPath+defFile);
}

void ReconstructionWidget::repopulateAlgorithmGroup()
{
	//std::cout << "repopulate" << std::endl;
	//StringOptionItem algoOption = mReconstructer->getSettings().getStringOption("Algorithm");
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

void ReconstructionWidget::currentDataComboIndexChanged(const QString& text)
{
  QDir dir = QFileInfo(mInputFile).dir();
  this->selectData(dir.filePath(text));
}

QString ReconstructionWidget::getCurrentPath()
{
  return QFileInfo(mInputFile).dir().absolutePath();
}

void ReconstructionWidget::reconstruct()
{
  ssc::messageManager()->sendInfo("Reconstructing...");
  qApp->processEvents();
  mReconstructer->reconstruct();
}

void ReconstructionWidget::updateComboBox()
{
  mDataComboBox->blockSignals(true);
  mDataComboBox->clear();

  QDir dir = QFileInfo(mInputFile).dir();
  QStringList nameFilters;
  nameFilters << "*.mhd";
  QStringList files = dir.entryList(nameFilters, QDir::Files);

  for (int i=0; i<files.size(); ++i)
  {
    mDataComboBox->addItem(files[i]);
  }
  mDataComboBox->setCurrentIndex(-1);
  for (int i=0; i<files.size(); ++i)
  {
    if (files[i]==QFileInfo(mInputFile).fileName())
      mDataComboBox->setCurrentIndex(i);
  }

  mDataComboBox->setToolTip(mInputFile);

  //mDataComboBox->addItem(mInputFile);
  mDataComboBox->blockSignals(false);

}

void ReconstructionWidget::reload()
{
  this->selectData(mInputFile);
}

void ReconstructionWidget::selectData(QString filename)
{
  if(filename.isEmpty())
  {
    ssc::messageManager()->sendWarning("no file selected");
    return;
  }

  mInputFile = filename;

  this->updateComboBox();
  mDataComboBox->setToolTip(mInputFile);

  // read data into reconstructer

  QStringList list = mInputFile.split("/");
  list[list.size()-1] = "";
  QString calFilesPath = list.join("/")+"/";
  mReconstructer->readFiles(mInputFile, calFilesPath);
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

void ReconstructionWidget::selectData()
{
  QString filename = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  getCurrentPath(),
                                  tr("USAcq (*.mhd)"));
  this->selectData(filename);
}
  
}//namespace
