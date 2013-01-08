#include "cxPrepareVesselsWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "cxColorSelectButton.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "cxRegistrationManager.h"
#include "cxDataLocations.h"

#include "cxPipelineWidget.h"
#include "sscHelperWidgets.h"
#include "sscColorDataAdapterXml.h"

#include "cxResampleImageFilter.h"
#include "cxSmoothingImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxBinaryThresholdImageFilter.h"

namespace cx
{
//------------------------------------------------------------------------------
PrepareVesselsWidget::PrepareVesselsWidget(RegistrationManagerPtr regManager, QWidget* parent) :
    RegistrationBaseWidget(regManager, parent, "PrepareVesselsWidget", "PrepareVesselsWidget")
{  
    ssc::XmlOptionFile options = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("registration").descend("PrepareVesselsWidget");
  // fill the pipeline with filters:
  mPipeline.reset(new Pipeline());
  FilterGroupPtr filters(new FilterGroup(options.descend("pipeline")));
  filters->append(FilterPtr(new ResampleImageFilter()));
  filters->append(FilterPtr(new SmoothingImageFilter()));
  filters->append(FilterPtr(new BinaryThresholdImageFilter()));
  filters->append(FilterPtr(new BinaryThinningImageFilter3DFilter()));
  mPipeline->initialize(filters);

//  mPipeline->getNodes()[0]->setValueName("US Image:");
//  mPipeline->getNodes()[0]->setHelp("Select an US volume acquired from the wire phantom.");
  mPipeline->setOption("Color", QVariant(QColor("red")));

  mLayout = new QVBoxLayout(this);

  mPipelineWidget = new PipelineWidget(NULL, mPipeline);
  mLayout->addWidget(mPipelineWidget);

  mColorDataAdapter = ssc::ColorDataAdapterXml::initialize("Color", "",
                                              "Color of all generated data.",
                                              QColor("green"), options.getElement());
  connect(mColorDataAdapter.get(), SIGNAL(changed()), this, SLOT(setColorSlot()));

  QPushButton* fixedButton = new QPushButton("Set as Fixed");
  fixedButton->setToolTip("Set output of centerline generation as the Fixed Volume in Registration");
  connect(fixedButton, SIGNAL(clicked()), this, SLOT(toFixedSlot()));
  QPushButton* movingButton = new QPushButton("Set as Moving");
  movingButton->setToolTip("Set output of centerline generation as the Moving Volume in Registration");
  connect(movingButton, SIGNAL(clicked()), this, SLOT(toMovingSlot()));

  QLayout* buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(fixedButton);
  buttonsLayout->addWidget(movingButton);

    mLayout->addWidget(ssc::createDataWidget(this, mColorDataAdapter));
  mLayout->addWidget(mPipelineWidget);
  mLayout->addStretch();
  mLayout->addLayout(buttonsLayout);
//  mLayout->addStretch();

  this->setColorSlot();

}

void PrepareVesselsWidget::setColorSlot()
{
  mPipeline->setOption(mColorDataAdapter->getValueName(), QVariant(mColorDataAdapter->getValue()));
}

void PrepareVesselsWidget::toMovingSlot()
{
    ssc::DataPtr data = mPipeline->getNodes().back()->getData();
  if (data)
    mManager->setMovingData(data);
}

void PrepareVesselsWidget::toFixedSlot()
{
  ssc::DataPtr data = mPipeline->getNodes().back()->getData();
  if (data)
    mManager->setFixedData(data);
}

PrepareVesselsWidget::~PrepareVesselsWidget()
{}

QString PrepareVesselsWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Segmentation and centerline extraction for the i2i registration.</h3>"
      "<p><i>Segment out blood vessels from the selected image, then extract the centerline."
      "When finished, set the result as moving or fixed data in the registration tab.</i></p>"
      "<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
      "</html>";
}

//------------------------------------------------------------------------------
}//namespace cx
