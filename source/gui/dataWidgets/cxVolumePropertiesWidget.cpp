#include "cxVolumePropertiesWidget.h"

#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "sscLabeledComboBoxWidget.h"
#include "sscLabeledLineEditWidget.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "cxTransferFunctionWidget.h"
#include "cxCroppingWidget.h"
#include "cxClippingWidget.h"
#include "cxShadingWidget.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxVolumeInfoWidget.h"
#include "sscVolumeHelpers.h"
#include "sscTypeConversions.h"

namespace cx
{

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveVolumeWidget::ActiveVolumeWidget(QWidget* parent) :
  BaseWidget(parent, "ActiveVolumeWidget", "Active Volume")
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(new DataSelectWidget(this, ActiveImageStringDataAdapter::New()));
}

QString ActiveVolumeWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Active volume</h3>"
      "<p>Displays the currently selected active volume.</p>"
      "<p><i>Use the list to change the active volume.</i></p>"
      "</html>";
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) :
		TabbedWidget(parent, "VolumePropertiesWidget", "Volume Properties")
{
  this->addTab(new VolumeInfoWidget(this), "Info");
  this->addTab(new TransferFunctionWidget(this), QString("Transfer Functions"));
  this->addTab(new ShadingWidget(this), "Shading");
  this->addTab(new CroppingWidget(this), "Crop");
  this->addTab(new ClippingWidget(this), "Clip");
}

QString VolumePropertiesWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Volume properties</h3>"
      "<p>Displays and adjusts information about a selected volume.</p>"
      "<p><i>Click the tabs to see what properties can be changed.</i></p>"
      "</html>";
}

}//namespace
