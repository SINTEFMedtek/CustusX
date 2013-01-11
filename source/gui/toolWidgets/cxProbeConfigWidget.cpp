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
	connect(mOrigin.get(), SIGNAL(changed()), this, SLOT(guiOriginSettingsChanged()));

	// define origin group
	Vector3DWidget* mOriginWidget = Vector3DWidget::createSmallHorizontal(this, mOrigin);
	mOriginWidget->showDim(2, false);

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
	sectorGroupBox->setToolTip("Define probe sector parameters.\nUnits in pixels and degrees.");
	QVBoxLayout* sectorLayout = new QVBoxLayout(sectorGroupBox);
	topLayout->addWidget(sectorGroupBox);

	sectorLayout->addWidget(mOriginWidget);
	mDepthWidget = new SliderRangeGroupWidget(this);
	mDepthWidget->setName("Depth");
	mDepthWidget->setRange(ssc::DoubleRange(0, 1000, 1));
	mDepthWidget->setDecimals(1);
	mDepthWidget->setToolTip("Define probe depth.\nUnits in pixels.");
	connect(mDepthWidget, SIGNAL(valueChanged(double, double)), this, SLOT(guiProbeSectorChanged()));
	sectorLayout->addWidget(mDepthWidget);

	mWidth = ssc::DoubleDataAdapterXml::initialize("width", "Width", "Width of probe sector", 0,
						ssc::DoubleRange(0, M_PI, M_PI/180), 0);
	mWidth->setInternal2Display(180.0/M_PI);
	connect(mWidth.get(), SIGNAL(changed()), this, SLOT(guiProbeSectorChanged()));
//	sectorLayout->addWidget(ssc::createDataWidget(this, mWidth));//
	sectorLayout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mWidth, 0, 0));

	// create buttons bar
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	topLayout->addLayout(buttonsLayout);

	mSyncBoxToSector = new QCheckBox("Sync Box to Sector", this);
	mSyncBoxToSector->setChecked(true);
	mSyncBoxToSector->setToolTip(""
		"Synchronize Crop Box to Probe Sector,\n"
		"changes in the sector will reset the crop box.");
	connect(mSyncBoxToSector, SIGNAL(toggled(bool)), this, SLOT(syncBoxToSectorChanged()));
	buttonsLayout->addWidget(mSyncBoxToSector);

//	this->createAction(this,
//	                QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-left-3.png"),
//					"Shift definition 1 pixel to the left.\nThis will shift the origin, crop box and sector.", "",
//	                SLOT(shiftLeftSlot()),
//	                buttonsLayout);
//	this->createAction(this,
//	                QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
//					"Shift definition 1 pixel to the right.\nThis will shift the origin, crop box and sector.", "",
//	                SLOT(shiftRightSlot()),
//	                buttonsLayout);

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

void ProbeConfigWidget::syncBoxToSectorChanged()
{

}

//void ProbeConfigWidget::shiftDefinition(ssc::Vector3D shift)
//{
//	// need a cx probe here, in order to set data.
//	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
//	if (!probe)
//		return;
//	ssc::ProbeData data = probe->getData();
//
//	ssc::ProbeData::ProbeImageData image = data.getImage();
//	image.mOrigin_p += shift;
//	for (int i=0; i<3; ++i)
//	{
//		image.mClipRect_p[2*i  ] += shift[i];
//		image.mClipRect_p[2*i+1] += shift[i];
//	}
//	data.setImage(image);
//
//	probe->setData(data);
//}
//
//void ProbeConfigWidget::shiftLeftSlot()
//{
//	this->shiftDefinition(ssc::Vector3D(-1, 0, 0));
//}
//
//void ProbeConfigWidget::shiftRightSlot()
//{
//	this->shiftDefinition(ssc::Vector3D( 1, 0, 0));
//}

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
	if (!mActiveProbeConfig->getTool())
		return;
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;

	// use the previously selected config as a suggestion for new config name.
	QString oldname = probe->getConfigName(probe->getConfigId());
	if (oldname.isEmpty())
		oldname = mLastKnownProbeConfigName;

	QString newName = QString("%1 (2)").arg(oldname);

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


	double sx = data.getImage().mSpacing[0]; // mm/pix
	double sy = data.getImage().mSpacing[1];

	mDepthWidget->setValue(std::make_pair(data.getDepthStart()/sy, data.getDepthEnd()/sy));
	mDepthWidget->setRange(ssc::DoubleRange(0, range.range()[1]*1.5, 1));

	mWidth->setValue(data.getWidth());

	if (data.getType()== ssc::ProbeData::tLINEAR)
	{
		mWidth->setValueRange(ssc::DoubleRange(0, range.range()[0]*1.5*sx, 1.0*sx));
		mWidth->setInternal2Display(1.0/sx);
	}
	if (data.getType()== ssc::ProbeData::tSECTOR)
	{
		mWidth->setValueRange(ssc::DoubleRange(0, M_PI, M_PI/180));
		mWidth->setInternal2Display(180.0/M_PI);
	}

	if (!probe->getConfigId().isEmpty())
	{
		mLastKnownProbeConfigName = probe->getConfigName(probe->getConfigId());
	}
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

	double sx = data.getImage().mSpacing[0]; // mm/pix
	double sy = data.getImage().mSpacing[1];

	data.setSector(mDepthWidget->getValue().first*sy, mDepthWidget->getValue().second*sy, mWidth->getValue());

	if (mSyncBoxToSector->isChecked())
		data.updateClipRectFromSector();

	probe->setData(data);

//	std::cout << "ProbeConfigWidget::guiProbeSectorChanged()" << std::endl;
}

void ProbeConfigWidget::guiImageSettingsChanged()
{
	if (mUpdating)
		return;
	// need a cx probe here, in order to set data.
	if (!mActiveProbeConfig->getTool())
		return;
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;
	ssc::ProbeData data = probe->getData();

	ssc::ProbeData::ProbeImageData image = data.getImage();

	image.mClipRect_p = mBBWidget->getValue();
	data.setImage(image);

//	if (mSyncBoxToSector->isChecked())
//		data.updateSectorFromClipRect();

	probe->setData(data);

//	std::cout << "ProbeConfigWidget::guiImageSettingsChanged()" << std::endl;
}

void ProbeConfigWidget::guiOriginSettingsChanged()
{
	if (mUpdating)
		return;
	// need a cx probe here, in order to set data.
	if (!mActiveProbeConfig->getTool())
		return;
	cx::ProbePtr probe = boost::shared_dynamic_cast<cx::Probe>(mActiveProbeConfig->getTool()->getProbe());
	if (!probe)
		return;
	ssc::ProbeData data = probe->getData();

	ssc::ProbeData::ProbeImageData image = data.getImage();

	// if sync: move clip rect accordingly
	if (mSyncBoxToSector->isChecked())
	{
		// shift
		ssc::Vector3D shift = mOrigin->getValue() - image.mOrigin_p;
		image.mClipRect_p = ssc::transform(ssc::createTransformTranslate(shift), image.mClipRect_p);
	}

	image.mOrigin_p = mOrigin->getValue();

	data.setImage(image);

	probe->setData(data);
}



}
