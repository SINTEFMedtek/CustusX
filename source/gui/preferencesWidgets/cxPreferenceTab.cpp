#include "cxPreferenceTab.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include "sscDoubleWidgets.h"
#include "sscHelperWidgets.h"
#include "cxSettings.h"
#include "cxViewManager.h"
#include "cxMultiVolume3DRepProducer.h"
#include "sscConfig.h"

namespace cx
{
//==============================================================================
// PreferencesTab
//------------------------------------------------------------------------------

PreferenceTab::PreferenceTab(QWidget *parent) :
    QWidget(parent)
    //settings()(settings())
{
  mTopLayout = new QVBoxLayout;

  QVBoxLayout* vtopLayout = new QVBoxLayout;
  vtopLayout->addLayout(mTopLayout);
  vtopLayout->setMargin(0);
  vtopLayout->addStretch();
  this->setLayout(vtopLayout);
}

//==============================================================================
// PerformanceTab
//------------------------------------------------------------------------------
PerformanceTab::PerformanceTab(QWidget *parent) :
		PreferenceTab(parent)
{
	mRenderingIntervalSpinBox = NULL;
	mRenderingRateLabel = NULL;
	mSmartRenderCheckBox = NULL;
	mGPU2DRenderCheckBox = NULL;
	mShadingCheckBox = NULL;
	mMainLayout = NULL;
}

void PerformanceTab::init()
{
  int renderingInterval = settings()->value("renderingInterval").toInt();

  QLabel* renderingIntervalLabel = new QLabel(tr("Rendering interval"));

  mRenderingIntervalSpinBox = new QSpinBox;
  mRenderingIntervalSpinBox->setSuffix("ms");
  mRenderingIntervalSpinBox->setMinimum(4);
  mRenderingIntervalSpinBox->setMaximum(1000);
  mRenderingIntervalSpinBox->setValue(renderingInterval);
  connect(mRenderingIntervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(renderingIntervalSlot(int)));

  mRenderingRateLabel = new QLabel("");
  this->renderingIntervalSlot(renderingInterval);

  double Mb = pow(10.0,6);
  bool ok = true;
  double maxRenderSize = settings()->value("View3D/maxRenderSize").toDouble(&ok);
  if (!ok)
    maxRenderSize = 10 * Mb;
  mMaxRenderSize = DoubleDataAdapterXml::initialize("MaxRenderSize", "Max Render Size (Mb)", "Maximum size of volumes used in volume rendering. Applies to new volumes.", maxRenderSize, DoubleRange(1*Mb,300*Mb,1*Mb), 0, QDomNode());
  mMaxRenderSize->setInternal2Display(1.0/Mb);

  double stillUpdateRate = settings()->value("stillUpdateRate").value<double>();
  mStillUpdateRate = DoubleDataAdapterXml::initialize("StillUpdateRate", "Still Update Rate", "Still Update Rate in vtkRenderWindow. Restart needed.", stillUpdateRate, DoubleRange(0.0001, 20, 0.0001), 4, QDomNode());

  mSmartRenderCheckBox = new QCheckBox("Smart Render");
  mSmartRenderCheckBox->setChecked(settings()->value("smartRender", true).toBool());
  mSmartRenderCheckBox->setToolTip("Render only when scene has changed, plus once per second.");

  m3DVisualizer = StringDataAdapterXml::initialize("ImageRender3DVisualizer",
	  "3D Renderer",
	  "Select 3D visualization method for images",
	  settings()->value("View3D/ImageRender3DVisualizer").toString(),
	  MultiVolume3DRepProducer::getAvailableVisualizers(),
	  QDomNode());
  m3DVisualizer->setDisplayNames(MultiVolume3DRepProducer::getAvailableVisualizerDisplayNames());

  bool useGPU2DRender = settings()->value("useGPU2DRendering").toBool();
	mGPU2DRenderCheckBox = new QCheckBox("Use GPU 2D Renderer");
	mGPU2DRenderCheckBox->setChecked(useGPU2DRender);
	mGPU2DRenderCheckBox->setToolTip("Use a GPU-based 2D renderer instead of the software-based one, if available.");

#ifndef USE_GLX_SHARED_CONTEXT
	mGPU2DRenderCheckBox->setChecked(false);
	mGPU2DRenderCheckBox->setEnabled(false);
#endif

//  bool useGPU3DDepthPeeling = settings()->value("View3D/depthPeeling").toBool();
//	mGPU3DDepthPeelingCheckBox = new QCheckBox("Use GPU 3D depth peeling");
//	mGPU3DDepthPeelingCheckBox->setChecked(useGPU3DDepthPeeling);
//	mGPU3DDepthPeelingCheckBox->setToolTip("Use a GPU-based 3D depth peeling to correctly visualize translucent surfaces.");

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(renderingIntervalLabel, 0, 0);
  new SpinBoxGroupWidget(this, mMaxRenderSize, mMainLayout, 1);
  mMainLayout->addWidget(mRenderingIntervalSpinBox, 0, 1);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
  mMainLayout->addWidget(mSmartRenderCheckBox, 2, 0);
  mMainLayout->addWidget(mGPU2DRenderCheckBox, 5, 0);
  new SpinBoxGroupWidget(this, mStillUpdateRate, mMainLayout, 7);
  mMainLayout->addWidget(sscCreateDataWidget(this, m3DVisualizer), 8, 0, 1, 2);

  mMainLayout->setColumnStretch(0, 0.6);
  mMainLayout->setColumnStretch(1, 0.6);
  mMainLayout->setColumnStretch(2, 0.3);

  mTopLayout->addLayout(mMainLayout);
}

void PerformanceTab::renderingIntervalSlot(int interval)
{
  mRenderingRateLabel->setText(QString("%1 fps").arg(1000.0/interval, 0, 'f', 1));
}

void PerformanceTab::saveParametersSlot()
{
  settings()->setValue("renderingInterval", mRenderingIntervalSpinBox->value());
  settings()->setValue("useGPU2DRendering", mGPU2DRenderCheckBox->isChecked());
  settings()->setValue("View3D/maxRenderSize",     mMaxRenderSize->getValue());
  settings()->setValue("smartRender",       mSmartRenderCheckBox->isChecked());
  settings()->setValue("stillUpdateRate",   mStillUpdateRate->getValue());
//  settings()->setValue("View3D/depthPeeling", mGPU3DDepthPeelingCheckBox->isChecked());
  settings()->setValue("View3D/ImageRender3DVisualizer",   m3DVisualizer->getValue());
}

} /* namespace cx */
