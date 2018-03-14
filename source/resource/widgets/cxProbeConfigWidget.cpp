/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxProbeConfigWidget.h>
#include <QGroupBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QCheckBox>
#include "cxLabeledComboBoxWidget.h"
#include "cxDoubleWidgets.h"
#include "cxToolProperty.h"
#include "cxVector3DWidget.h"
#include "cxDoubleSpanSlider.h"
#include "cxHelperWidgets.h"
#include "cxTypeConversions.h"
#include "cxDoublePairProperty.h"
#include "cxVisServices.h"

namespace cx
{

ProbeConfigWidget::ProbeConfigWidget(VisServicesPtr services, QWidget* parent) : BaseWidget(parent, "probe_config_widget", "Probe Configuration")
{
	mServices = services;
	mUpdating = false;
	this->setToolTip("Edit ultrasound probe configuration");

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	TrackingServicePtr ts = mServices->tracking();
	mActiveProbeConfig = StringPropertyActiveProbeConfiguration::New(ts);
	connect(mActiveProbeConfig.get(), &StringPropertyActiveProbeConfiguration::changed, this, &ProbeConfigWidget::activeProbeConfigurationChangedSlot);
	mActiveProbeConfigWidget = new LabeledComboBoxWidget(this, mActiveProbeConfig);
	topLayout->addWidget(mActiveProbeConfigWidget);

	mOrigin = Vector3DProperty::initialize("Origin",
		"Origin",
		"Origin of tool space in the probe image.\nUnits in pixels.",
		Vector3D(0,0,0),
		DoubleRange(-1000,1000,1),
		1,
		QDomNode());
	connect(mOrigin.get(), SIGNAL(changed()), this, SLOT(guiOriginSettingsChanged()));

	// define origin group
	Vector3DWidget* mOriginWidget = Vector3DWidget::createSmallHorizontal(this, mOrigin);
	mOriginWidget->showDim(2, false);

	// define cropping group
	QGroupBox* cropGroupBox = new QGroupBox("Crop Box", this);
	cropGroupBox->setToolTip("Define cropping box for the probe image.\nUnits in pixels.");
	QVBoxLayout* cropLayout = new QVBoxLayout(cropGroupBox);
	topLayout->addWidget(cropGroupBox);

	QStringList bbCaptions = QStringList() << "X (pixels)" << "Y (pixels)";
	mBBWidget = new BoundingBoxWidget(this, bbCaptions);
	cropLayout->addWidget(mBBWidget);
	connect(mBBWidget, &BoundingBoxWidget::changed, this, &ProbeConfigWidget::guiImageSettingsChanged);

	// create sector group
	QGroupBox* sectorGroupBox = new QGroupBox("Sector");
	sectorGroupBox->setToolTip("Define probe sector parameters.\nUnits in pixels and degrees.");
	QVBoxLayout* sectorLayout = new QVBoxLayout(sectorGroupBox);
	topLayout->addWidget(sectorGroupBox);

	sectorLayout->addWidget(mOriginWidget);
	DoublePairPropertyPtr depthProperty = DoublePairProperty::initialize("Depth", "Depth", "Define probe depth.\nUnits in pixels.", DoubleRange(0, 1000, 1), 1);
	mDepthWidget = new SliderRangeGroupWidget(this, depthProperty);
	connect(mDepthWidget, SIGNAL(valueChanged(double, double)), this, SLOT(guiProbeSectorChanged()));
	sectorLayout->addWidget(mDepthWidget);

	mWidth = DoubleProperty::initialize("width", "Width", "Width of probe sector", 0,
						DoubleRange(0, M_PI, M_PI/180), 0);
	mWidth->setInternal2Display(180.0/M_PI);
	connect(mWidth.get(), SIGNAL(changed()), this, SLOT(guiProbeSectorChanged()));
	sectorLayout->addWidget(new SpinBoxAndSliderGroupWidget(this, mWidth, 0, 0));

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

	buttonsLayout->addStretch();
	this->createAction(this,
	                QIcon(":/icons/preset_remove.png"),
					"Delete the current probe config", "",
	                SLOT(deletePresetSlot()),
	                buttonsLayout);

	this->createAction(this,
		            QIcon(":/icons/preset_save.png"),
					"Add the current setting as a probe config", "",
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

void ProbeConfigWidget::savePresetSlot()
{
	if (!mActiveProbeConfig->getTool())
		return;
	ProbePtr probe = mActiveProbeConfig->getTool()->getProbe();
//	ProbeImplPtr probe = boost::dynamic_pointer_cast<ProbeImpl>(mActiveProbeConfig->getTool()->getProbe());
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
	ProbePtr probe = mActiveProbeConfig->getTool()->getProbe();
//	ProbeImplPtr probe = boost::dynamic_pointer_cast<ProbeImpl>(mActiveProbeConfig->getTool()->getProbe());
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

	probe->removeCurrentConfig();
}


void ProbeConfigWidget::activeProbeConfigurationChangedSlot()
{
	cx::ProbePtr probe = mActiveProbeConfig->getTool()->getProbe();
	if (!probe)
		return;
	ProbeDefinition data = probe->getProbeDefinition();
	mUpdating= true;

	DoubleBoundingBox3D range(0, data.getSize().width(), 0, data.getSize().height());
	mBBWidget->setValue(data.getClipRect_p(), range);

	mOrigin->setValue(data.getOrigin_p());

	double sx = data.getSpacing()[0]; // mm/pix
	double sy = data.getSpacing()[1];

	mDepthWidget->setValue(data.getDepthStart()/sy, data.getDepthEnd()/sy);
	mDepthWidget->setRange(DoubleRange(0, range.range()[1]*1.5, 1));

	mWidth->setValue(data.getWidth());

	if (data.getType()== ProbeDefinition::tLINEAR)
	{
		mWidth->setValueRange(DoubleRange(0, range.range()[0]*1.5*sx, 1.0*sx));
		mWidth->setInternal2Display(1.0/sx);
	}
	if (data.getType()== ProbeDefinition::tSECTOR)
	{
		mWidth->setValueRange(DoubleRange(0, M_PI, M_PI/180));
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
	if(!mActiveProbeConfig->getTool())
		return;
	// need a cx probe here, in order to set data.
	cx::ProbePtr probe = mActiveProbeConfig->getTool()->getProbe();
	if (!probe)
		return;
	ProbeDefinition data = probe->getProbeDefinition();

//	double sx = data.getSpacing()[0]; // mm/pix
	double sy = data.getSpacing()[1];

	data.setSector(mDepthWidget->getValue().first*sy, mDepthWidget->getValue().second*sy, mWidth->getValue());

	if (mSyncBoxToSector->isChecked())
		data.updateClipRectFromSector();

	probe->setProbeDefinition(data);
}

void ProbeConfigWidget::guiImageSettingsChanged()
{
	if (mUpdating)
		return;
	// need a cx probe here, in order to set data.
	if (!mActiveProbeConfig->getTool())
		return;
	cx::ProbePtr probe = mActiveProbeConfig->getTool()->getProbe();
	if (!probe)
		return;
	ProbeDefinition data = probe->getProbeDefinition();

	data.setClipRect_p(mBBWidget->getValue());

	probe->setProbeDefinition(data);
}

void ProbeConfigWidget::guiOriginSettingsChanged()
{
	if (mUpdating)
		return;
	// need a cx probe here, in order to set data.
	if (!mActiveProbeConfig->getTool())
		return;
	cx::ProbePtr probe = mActiveProbeConfig->getTool()->getProbe();
	if (!probe)
		return;
	ProbeDefinition data = probe->getProbeDefinition();

	// if sync: move clip rect accordingly
	if (mSyncBoxToSector->isChecked())
	{
		// shift
		Vector3D shift = mOrigin->getValue() - data.getOrigin_p();
		data.setClipRect_p(transform(createTransformTranslate(shift), data.getClipRect_p()));
	}

	data.setOrigin_p(mOrigin->getValue());

	probe->setProbeDefinition(data);
}



}
