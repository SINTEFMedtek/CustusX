// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include <cxProbeConfigWidget.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include "sscLabeledComboBoxWidget.h"
#include "sscDoubleWidgets.h"
#include "cxToolDataAdapters.h"
#include "cxVector3DWidget.h"
#include "cxProbe.h"
#include "cxDoubleSpanSlider.h"
#include "sscHelperWidgets.h"
#include "sscTypeConversions.h"

namespace cx
{

ProbeConfigWidget::ProbeConfigWidget(QWidget* parent) : BaseWidget(parent, "ProbeConfigWidget", "Probe Configuration")
{
	mUpdating = false;
//	this->setStatusTip(this->defaultWhatsThis());
	this->setToolTip(this->defaultWhatsThis());

	QVBoxLayout* topLayout = new QVBoxLayout(this);
//	topLayout->addWidget(new QLabel("Probe!!!!"));
	mActiveProbeConfig = ActiveProbeConfigurationStringDataAdapter::New();
	connect(mActiveProbeConfig.get(), SIGNAL(changed()), this, SLOT(activeProbeConfigurationChangedSlot()));
	mActiveProbeConfigWidget = new ssc::LabeledComboBoxWidget(this, mActiveProbeConfig);
	topLayout->addWidget(mActiveProbeConfigWidget);

	mOrigin = ssc::Vector3DDataAdapterXml::initialize("Origin",
		"Origin",
		"Origin of tool space in the probe image.\nUnits in pixels.",
		ssc::Vector3D(0,0,0),
		ssc::DoubleRange(-1000,1000,1),
		1,
		QDomNode());
	connect(mOrigin.get(), SIGNAL(changed()), this, SLOT(guiImageSettingsChanged()));

	// define origin group
	Vector3DWidget* mOriginWidget = Vector3DWidget::createSmallHorizontal(this, mOrigin);
	mOriginWidget->showDim(2, false);
	topLayout->addWidget(mOriginWidget);

	// define cropping group
	QGroupBox* cropGroupBox = new QGroupBox("Crop Box");
	cropGroupBox->setToolTip("Define cropping box for the probe image.\nUnits in pixels.");
	QVBoxLayout* cropLayout = new QVBoxLayout(cropGroupBox);
	topLayout->addWidget(cropGroupBox);

	mBBWidget = new BoundingBoxWidget(this);
	mBBWidget->showDim(2, false);
	cropLayout->addWidget(mBBWidget);
	connect(mBBWidget, SIGNAL(changed()), this, SLOT(guiImageSettingsChanged()));

	// create sector group
	QGroupBox* sectorGroupBox = new QGroupBox("Sector");
	sectorGroupBox->setToolTip("Define probe sector parameters.\nUnits in mm.");
	QVBoxLayout* sectorLayout = new QVBoxLayout(sectorGroupBox);
	topLayout->addWidget(sectorGroupBox);

	mDepthWidget = new SliderRangeGroupWidget(this);
	mDepthWidget->setName("Depth");
	mDepthWidget->setRange(ssc::DoubleRange(0, 100, 1));
	connect(mDepthWidget, SIGNAL(valueChanged(double, double)), this, SLOT(guiProbeSectorChanged()));
	sectorLayout->addWidget(mDepthWidget);

	mWidth = ssc::DoubleDataAdapterXml::initialize("width", "Width", "Width of probe sector", 0,
						ssc::DoubleRange(0, M_PI, M_PI/180), 0);
	mWidth->setInternal2Display(180.0/M_PI);
	connect(mWidth.get(), SIGNAL(changed()), this, SLOT(guiProbeSectorChanged()));
	sectorLayout->addWidget(ssc::createDataWidget(this, mWidth));

	// create buttons bar
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	topLayout->addLayout(buttonsLayout);

	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-left-3.png"),
					"Shift definition 1 pixel to the left.\nThis will shift the origin, crop box and sector.", "",
	                SLOT(shiftLeftSlot()),
	                buttonsLayout);
	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
					"Shift definition 1 pixel to the right.\nThis will shift the origin, crop box and sector.", "",
	                SLOT(shiftRightSlot()),
	                buttonsLayout);

	buttonsLayout->addStretch();
	this->createAction(this,
	                QIcon(":/icons/preset_remove.png"),
					"Delete the current preset", "",
	                SLOT(deletePresetSlot()),
	                buttonsLayout);

	this->createAction(this,
		            QIcon(":/icons/preset_save.png"),
					"Add the current setting as a preset", "",
	                SLOT(savePresetSlot()),
	                buttonsLayout);

	topLayout->addStretch();
}

ProbeConfigWidget::~ProbeConfigWidget()
{
}

void ProbeConfigWidget::shiftDefinition(ssc::Vector3D shift)
{
	// need a cx probe here, in order to set data.
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;
	ssc::ProbeData data = probe->getData();

	ssc::ProbeData::ProbeImageData image = data.getImage();
	image.mOrigin_p += shift;
	for (int i=0; i<3; ++i)
	{
		image.mClipRect_p[2*i  ] += shift[i];
		image.mClipRect_p[2*i+1] += shift[i];
	}
	data.setImage(image);

	probe->setProbeSector(data);
}

void ProbeConfigWidget::shiftLeftSlot()
{
	this->shiftDefinition(ssc::Vector3D(-1, 0, 0));
}

void ProbeConfigWidget::shiftRightSlot()
{
	this->shiftDefinition(ssc::Vector3D( 1, 0, 0));
}

QString ProbeConfigWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Probe Configuration</h3>"
      "<p>"
      "View and edit the probe configuration. "
      "Use the origin to define the position of the probe image in relation "
      "to the tool t space. Set the cropping box and the probe sector parameters "
      "(depth and width). All can be written back to the ProbeCalibConfigs.xml file."
      "</p>"
      "</html>";
}

void ProbeConfigWidget::savePresetSlot()
{
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;

	QString newName = QString("%1 (2)").arg(probe->getConfigName(probe->getConfigId()));

    bool ok;
    newName = QInputDialog::getText(this, "Save Config",
                                         "Config Name", QLineEdit::Normal,
                                         newName, &ok);
    if (!ok || newName.isEmpty())
      return;

    QStringList existingConfigs = probe->getConfigIdList();
    QString newUid;

    // is name exists: overwrite.
    for (int i=0; i<existingConfigs.size(); ++i)
    {
    	if (newName==probe->getConfigName(existingConfigs[i]))
    	{
    		newUid = existingConfigs[i];
    	}
    }

    if (newUid.isEmpty())
    {
    	newUid = newName;
//    	Width: Max, Depth: 45 mm
    	newUid.remove(QRegExp("(:|\\s|\\(|\\)|,)")); // remove fancy characters from uid
    	QString root = newUid;
    	int i=2;
    	while (existingConfigs.contains(newUid))
    	{
    		newUid = QString("%1_%2").arg(root).arg(i++);
    	}
    }

    std::cout << QString("Save probe config uid=%1, name=%2").arg(newUid).arg(newName) << std::endl;
    probe->saveCurrentConfig(newUid, newName);
}

void ProbeConfigWidget::deletePresetSlot()
{
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;

	QString message = QString("Do you really want to delete configuration\n%1?").arg(probe->getConfigName(probe->getConfigId()));
	if (QMessageBox::warning(this,
					"Delete Config",
					message,
					QMessageBox::No | QMessageBox::Yes) != QMessageBox::Yes)
	{
		return;
	}

//    std::cout << QString("Delete probe config uid=%1").arg(probe->getConfigId()) << std::endl;
	probe->removeCurrentConfig();
}


void ProbeConfigWidget::activeProbeConfigurationChangedSlot()
{
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;
	ssc::ProbeData data = probe->getData();
	mUpdating= true;

	ssc::DoubleBoundingBox3D range(0, data.getImage().mSize.width(), 0, data.getImage().mSize.height());
	mBBWidget->setValue(data.getImage().mClipRect_p, range);

	mOrigin->setValue(data.getImage().mOrigin_p);

	mDepthWidget->setValue(std::make_pair(data.getDepthStart(), data.getDepthEnd()));
	mWidth->setValue(data.getWidth());

	if (data.getType()== ssc::ProbeData::tLINEAR)
	{
		mWidth->setValueRange(ssc::DoubleRange(0, 1000, 1));
		mWidth->setInternal2Display(1);
	}
	if (data.getType()== ssc::ProbeData::tSECTOR)
	{
		mWidth->setValueRange(ssc::DoubleRange(0, M_PI, M_PI/180));
		mWidth->setInternal2Display(180.0/M_PI);
	}

//	std::cout << "ProbeConfigWidget::activeProbeConfigurationChangedSlot()" << std::endl;
	mUpdating= false;
}


void ProbeConfigWidget::guiProbeSectorChanged()
{
	if (mUpdating)
		return;
	// need a cx probe here, in order to set data.
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;
	ssc::ProbeData data = probe->getData();

	data.setSector(mDepthWidget->getValue().first, mDepthWidget->getValue().second, mWidth->getValue());
	data.updateClipRectFromSector();

	probe->setProbeSector(data);

//	std::cout << "ProbeConfigWidget::guiProbeSectorChanged()" << std::endl;
}

void ProbeConfigWidget::guiImageSettingsChanged()
{
	if (mUpdating)
		return;
	// need a cx probe here, in order to set data.
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;
	ssc::ProbeData data = probe->getData();

	ssc::ProbeData::ProbeImageData image = data.getImage();
	image.mOrigin_p = mOrigin->getValue();
	image.mClipRect_p = mBBWidget->getValue();
	data.setImage(image);
	data.updateSectorFromClipRect();

	probe->setProbeSector(data);

//	std::cout << "ProbeConfigWidget::guiImageSettingsChanged()" << std::endl;
}




}
