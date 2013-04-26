#include "cxMeshInfoWidget.h"

#include <QVBoxLayout>
#include <QColorDialog>
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

//void MeshPropertiesWidget::deleteDataSlot()
//{
//  if(!mMesh)
//    return;
//  ssc::dataManager()->removeData(mMesh->getUid());
//}

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
  
//  QColor result = QColorDialog::getColor( mMesh->getColor(), this, "Select Mesh Color", QColorDialog::ShowAlphaChannel);
//  if (result.isValid() && result != mMesh->getColor())
//  {
//    mMesh->setColor(result);
//  }
  mMesh->setColor(mColorAdapter->getValue());
}

void MeshInfoWidget::meshSelectedSlot()
{
//	mMeshPropertiesGroupBox->setEnabled(false);

	if (mMesh == mSelectMeshWidget->getMesh())
	return;

	if(mMesh)
	{
		disconnect(mBackfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setBackfaceCullingSlot(bool)));
		disconnect(mFrontfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setFrontfaceCullingSlot(bool)));
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
	}

	mMesh = mSelectMeshWidget->getMesh();
//	mMeshPropertiesGroupBox->setEnabled(mMesh!=0);

	if (!mMesh)
	return;

	mBackfaceCullingCheckBox->setChecked(mMesh->getBackfaceCulling());
	mFrontfaceCullingCheckBox->setChecked(mMesh->getFrontfaceCulling());
	connect(mBackfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setBackfaceCullingSlot(bool)));
	connect(mFrontfaceCullingCheckBox, SIGNAL(toggled(bool)), mMesh.get(), SLOT(setFrontfaceCullingSlot(bool)));
	connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));

	mParentFrameAdapter->setData(mMesh);
	mNameAdapter->setData(mMesh);
	mUidAdapter->setData(mMesh);
	mColorAdapter->setValue(mMesh->getColor());

	std::map<std::string, std::string> info = ssc::getDisplayFriendlyInfo(mMesh);
	this->populateTableWidget(info);
}

void MeshInfoWidget::importTransformSlot()
{
  if(!mMesh)
    return;
  ssc::DataPtr parent = ssc::dataManager()->getData(mMesh->getParentSpace());
  if (!parent)
    return;
  mMesh->get_rMd_History()->setRegistration(parent->get_rMd());
  ssc::messageManager()->sendInfo("Assigned rMd from volume [" + parent->getName() + "] to surface [" + mMesh->getName() + "]");
}
  
void MeshInfoWidget::meshChangedSlot()
{
	mBackfaceCullingCheckBox->setChecked(mMesh->getBackfaceCulling());
	mFrontfaceCullingCheckBox->setChecked(mMesh->getFrontfaceCulling());
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
	//  this->setObjectName("MeshPropertiesWidget");
	//  this->setWindowTitle("Mesh Properties");

	  mSelectMeshWidget = SelectMeshStringDataAdapter::New();
	  mSelectMeshWidget->setValueName("Surface: ");
	  connect(mSelectMeshWidget.get(), SIGNAL(changed()), this, SLOT(meshSelectedSlot()));

	  //layout
	//  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

//	  toptopLayout->addWidget(new DataSelectWidget(this, mSelectMeshWidget));
//	  toptopLayout->addWidget(mMeshPropertiesGroupBox);
	//  QGridLayout* gridLayout = new QGridLayout(mMeshPropertiesGroupBox);

	ssc::XmlOptionFile options = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("MeshInfoWidget");
//	ssc::XmlOptionFile root = options.descend("root");
	QString uid("Color");
	QString name("");
	QString help("Color of output model.");
	QColor color("red");
	if(mSelectMeshWidget->getMesh())
		color = mSelectMeshWidget->getMesh()->getColor();
	mColorAdapter = ssc::ColorDataAdapterXml::initialize(
			uid, name, help, color, options.getElement());
//	QPushButton* chooseColor = new QPushButton("Choose color...", this);
//	connect(chooseColor, SIGNAL(clicked()), this, SLOT(setColorSlot()));
	connect(mColorAdapter.get(), SIGNAL(changed()), this, SLOT(setColorSlot()));

	QPushButton* importTransformButton = new QPushButton("Import Transform from Parent", this);
	importTransformButton->setToolTip("Replace data transform with that of the parent data.");
	connect(importTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));

	//  QPushButton* deleteButton = new QPushButton("Delete", this);
	//  deleteButton->setToolTip("Remove the selected surface from the system.");
	//  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDataSlot()));

	int position = 1;

	mUidAdapter = DataUidEditableStringDataAdapter::New();
	mNameAdapter = DataNameEditableStringDataAdapter::New();
	mParentFrameAdapter = ParentFrameStringDataAdapter::New();

	gridLayout->addWidget(new DataSelectWidget(this, mSelectMeshWidget), position++, 0, 1, 2);
//	gridLayout->addWidget(mMeshPropertiesGroupBox, position++, 0, 1, 2);
	new ssc::LabeledLineEditWidget(this, mUidAdapter, gridLayout, position++);
	new ssc::LabeledLineEditWidget(this, mNameAdapter, gridLayout, position++);
	new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, position++);

	QWidget* optionsWidget = new QWidget(this);
	QGridLayout* optionsLayout = new QGridLayout(optionsWidget);
	gridLayout->addWidget(optionsWidget, position++, 0, 1, 2);
	mBackfaceCullingCheckBox = new QCheckBox("Backface culling");
	mBackfaceCullingCheckBox->setToolTip("Set backface culling on. This makes transparent meshes work, but only draws outside mesh walls (eg. navigating inside meshes will not work).");
	optionsLayout->addWidget(mBackfaceCullingCheckBox, position++, 0);
	mFrontfaceCullingCheckBox = new QCheckBox("Frontface culling");
	mFrontfaceCullingCheckBox->setToolTip("Set frontface culling on. Can be used to make transparent meshes work from inside the meshes.");
	optionsLayout->addWidget(mFrontfaceCullingCheckBox, position++, 0);

	//  gridLayout->addWidget(deleteButton, position++, 0, 1, 2);
	optionsLayout->addWidget(ssc::createDataWidget(this, mColorAdapter), position, 0);

	gridLayout->addWidget(mTabelWidget, position++, 0, 1, 2);

	gridLayout->addWidget(importTransformButton, position++, 0, 1, 2);

	this->addStretch();
}

}//end namespace cx
