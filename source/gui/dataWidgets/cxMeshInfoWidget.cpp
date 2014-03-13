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
#include "cxMeshInfoWidget.h"

#include <QVBoxLayout>
#include "sscImage.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscLabeledLineEditWidget.h"
#include "sscMeshHelpers.h"
#include "sscHelperWidgets.h"
#include "sscColorDataAdapterXml.h"
#include "cxDataLocations.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "sscLogger.h"


namespace cx
{


MeshInfoWidget::MeshInfoWidget(QWidget* parent) :
		InfoWidget(parent, "MeshInfoWidget", "Mesh Properties")//, mMeshPropertiesGroupBox(new QGroupBox(this))
{
	this->addWidgets();
	this->meshSelectedSlot();
}

MeshInfoWidget::~MeshInfoWidget()
{}

void MeshInfoWidget::setColorSlot()
{
  if(!mMesh)
    return;
  // Implement like TransferFunctionColorWidget::setColorSlot()
  // to prevent crash problems
  QTimer::singleShot(1, this, SLOT(setColorSlotDelayed()));
}

void MeshInfoWidget::setColorSlotDelayed()
{
  mMesh->setColor(mColorAdapter->getValue());
}

void MeshInfoWidget::meshSelectedSlot()
{
	if (mMesh == mSelectMeshWidget->getMesh())
	return;

	if(mMesh)
	{
		disconnect(mBackfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setBackfaceCullingSlot(bool)));
		disconnect(mFrontfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setFrontfaceCullingSlot(bool)));
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
	}

	mMesh = mSelectMeshWidget->getMesh();

	if (!mMesh)
	{
		mParentFrameAdapter->setData(mMesh);
		mNameAdapter->setData(mMesh);
		mUidAdapter->setData(mMesh);
		return;
	}

	mBackfaceCullingCheckBox->setChecked(mMesh->getBackfaceCulling());
	mFrontfaceCullingCheckBox->setChecked(mMesh->getFrontfaceCulling());
	connect(mBackfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setBackfaceCullingSlot(bool)));
	connect(mFrontfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setFrontfaceCullingSlot(bool)));
	connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));

	mParentFrameAdapter->setData(mMesh);
	mNameAdapter->setData(mMesh);
	mUidAdapter->setData(mMesh);
	mColorAdapter->setValue(mMesh->getColor());

	std::map<std::string, std::string> info = getDisplayFriendlyInfo(mMesh);
	this->populateTableWidget(info);
}

void MeshInfoWidget::importTransformSlot()
{
  if(!mMesh)
    return;
  DataPtr parent = dataManager()->getData(mMesh->getParentSpace());
  if (!parent)
    return;
  mMesh->get_rMd_History()->setRegistration(parent->get_rMd());
  messageManager()->sendInfo("Assigned rMd from volume [" + parent->getName() + "] to surface [" + mMesh->getName() + "]");
}
  
void MeshInfoWidget::meshChangedSlot()
{
	mBackfaceCullingCheckBox->setChecked(mMesh->getBackfaceCulling());
	mFrontfaceCullingCheckBox->setChecked(mMesh->getFrontfaceCulling());
	mColorAdapter->setValue(mMesh->getColor());
}

void MeshInfoWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void MeshInfoWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void MeshInfoWidget::addWidgets()
{
	mSelectMeshWidget = SelectMeshStringDataAdapter::New();
	mSelectMeshWidget->setValueName("Surface: ");
	connect(mSelectMeshWidget.get(), SIGNAL(changed()), this, SLOT(meshSelectedSlot()));

	XmlOptionFile options = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("MeshInfoWidget");
	QString uid("Color");
	QString name("");
	QString help("Color of the mesh.");
	QColor color("red");

	if(mSelectMeshWidget->getMesh())
		color = mSelectMeshWidget->getMesh()->getColor();

	mColorAdapter = ColorDataAdapterXml::initialize(uid, name, help, color, options.getElement());
	connect(mColorAdapter.get(), SIGNAL(changed()), this, SLOT(setColorSlot()));

	QPushButton* importTransformButton = new QPushButton("Import Transform from Parent", this);
	importTransformButton->setToolTip("Replace data transform with that of the parent data.");
	connect(importTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));

	mUidAdapter = DataUidEditableStringDataAdapter::New();
	mNameAdapter = DataNameEditableStringDataAdapter::New();
	mParentFrameAdapter = ParentFrameStringDataAdapter::New();

	QWidget* optionsWidget = new QWidget(this);
	QHBoxLayout* optionsLayout = new QHBoxLayout(optionsWidget);
	mBackfaceCullingCheckBox = new QCheckBox("Backface culling");
	mBackfaceCullingCheckBox->setToolTip("Set backface culling on. This makes transparent meshes work, but only draws outside mesh walls (eg. navigating inside meshes will not work).");
	optionsLayout->addWidget(mBackfaceCullingCheckBox);
	mFrontfaceCullingCheckBox = new QCheckBox("Frontface culling");
	mFrontfaceCullingCheckBox->setToolTip("Set frontface culling on. Can be used to make transparent meshes work from inside the meshes.");
	optionsLayout->addWidget(mFrontfaceCullingCheckBox);
	optionsLayout->addWidget(sscCreateDataWidget(this, mColorAdapter));
	optionsLayout->addStretch(1);

	int gridLayoutRow = 1;

	gridLayout->addWidget(new DataSelectWidget(this, mSelectMeshWidget), gridLayoutRow++, 0, 1, 2);
	new LabeledLineEditWidget(this, mUidAdapter, gridLayout, gridLayoutRow++);
	new LabeledLineEditWidget(this, mNameAdapter, gridLayout, gridLayoutRow++);
	new LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, gridLayoutRow++);
	gridLayout->addWidget(optionsWidget, gridLayoutRow++, 0, 1, 2);
	gridLayout->addWidget(mTableWidget, gridLayoutRow++, 0, 1, 2);
	gridLayout->addWidget(importTransformButton, gridLayoutRow++, 0, 1, 2);

	this->addStretch();
}

}//end namespace cx
