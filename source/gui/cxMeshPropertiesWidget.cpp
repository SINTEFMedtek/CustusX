#include "cxMeshPropertiesWidget.h"

#include <QVBoxLayout>
#include <QColorDialog>
#include "sscImage.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscLabeledLineEditWidget.h"
#include "cxDataInterface.h"

namespace cx
{


MeshPropertiesWidget::MeshPropertiesWidget(QWidget* parent) :
  QWidget(parent),
//  mMeshComboBox(new QComboBox(this)),
  mMeshPropertiesGroupBox(new QGroupBox(this))
{
  this->setObjectName("MeshPropertiesWidget");
  this->setWindowTitle("Surface Properties");

//  this->visibilityChangedSlot(true);
  
  mSelectMeshWidget = SelectMeshStringDataAdapter::New();
  mSelectMeshWidget->setValueName("Surface: ");
  connect(mSelectMeshWidget.get(), SIGNAL(meshChanged(QString)), this, SLOT(meshSelectedSlot(const QString&)));

//  //combobox
//  mMeshComboBox->setEditable(false);
//  mMeshComboBox->setEnabled(false);
//  connect(mMeshComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(meshSelectedSlot(const QString&)));
  
  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

//  toptopLayout->addWidget(mMeshComboBox);
  toptopLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mSelectMeshWidget));

  toptopLayout->addWidget(mMeshPropertiesGroupBox);
  QGridLayout* gridLayout = new QGridLayout(mMeshPropertiesGroupBox);

  QPushButton* chooseColor = new QPushButton("Choose color...", this);
  connect(chooseColor, SIGNAL(clicked()), this, SLOT(setColorSlot()));
  
  QPushButton* importTransformButton = new QPushButton("Import Transform from Parent", this);
  importTransformButton->setToolTip("Replace data transform with that of the parent data.");
  connect(importTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));

  QPushButton* deleteButton = new QPushButton("Delete", this);
  deleteButton->setToolTip("Remove the selected surface from the system.");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDataSlot()));
  
  int position = 1;
  
  mUidAdapter = DataUidEditableStringDataAdapter::New();
  ssc::LabeledLineEditWidget*  uidEdit = new ssc::LabeledLineEditWidget(this, mUidAdapter);
  gridLayout->addWidget(uidEdit, position++, 0);

  mNameAdapter = DataNameEditableStringDataAdapter::New();
  ssc::LabeledLineEditWidget*  nameEdit = new ssc::LabeledLineEditWidget(this, mNameAdapter);
  gridLayout->addWidget(nameEdit, position++, 0);

  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
  gridLayout->addWidget(combo, position++, 0);

  gridLayout->addWidget(deleteButton, position++, 0);

  gridLayout->addWidget(chooseColor, position++, 0);
  gridLayout->addWidget(importTransformButton, position++, 0);

  toptopLayout->addStretch();

  this->meshSelectedSlot("...");
}

MeshPropertiesWidget::~MeshPropertiesWidget()
{
}

void MeshPropertiesWidget::deleteDataSlot()
{
  if(!mMesh)
    return;
  ssc::dataManager()->removeData(mMesh->getUid());
}

void MeshPropertiesWidget::importTransformSlot()
{
  if(!mMesh)
    return;
  ssc::DataPtr parent = ssc::dataManager()->getData(mMesh->getParentFrame());
  if (!parent)
    return;
  mMesh->get_rMd_History()->setRegistration(parent->get_rMd());
  ssc::messageManager()->sendInfo("Assigned rMd from volume [" + parent->getName() + "] to surface [" + mMesh->getName() + "]");
}

void MeshPropertiesWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void MeshPropertiesWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void MeshPropertiesWidget::setColorSlot()
{
  if(!mMesh)
    return;
  // Implement like TransferFunctionColorWidget::setColorSlot()
  // to prevent crash problems
  QTimer::singleShot(1, this, SLOT(setColorSlotDelayed()));
}

void MeshPropertiesWidget::setColorSlotDelayed()
{
  
  QColor result = QColorDialog::getColor( mMesh->getColor(), this, "Select Mesh Color", QColorDialog::ShowAlphaChannel);
  if (result.isValid() && result != mMesh->getColor())
  {
    mMesh->setColor(result);
  }
}

//void MeshPropertiesWidget::visibilityChangedSlot(bool visible)
//{
//  if(visible)
//  {
//    connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateMeshComboBoxSlot()));
//    this->populateMeshComboBoxSlot();
//  }
//  else
//  {
//    disconnect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateMeshComboBoxSlot()));
//  }
//}
  
//// mMeshCombobox duplicates the functionality of the combobox in the
//// cxContectDockWidget. Should be merged with this later.
//void MeshPropertiesWidget::populateMeshComboBoxSlot()
//{
//  //mMeshComboBox->blockSignals(true);
//  mMeshComboBox->clear();
//
//  //get a list of meshes from the datamanager
//  std::map<QString, ssc::MeshPtr> meshes = ssc::dataManager()->getMeshes();
//  if(meshes.size() == 0)
//  {
//    mMeshComboBox->insertItem(1, QString("Import a mesh to begin..."));
//    mMeshComboBox->setEnabled(false);
//    return;
//  }
//
//  mMeshComboBox->setEnabled(true);
//
//  //add these to the combobox
//  typedef std::map<QString, ssc::MeshPtr>::iterator iterator;
//  mMeshComboBox->insertItem(1, QString("<No mesh selected>"));
//  int listPosition = 2;
//  for(iterator i = meshes.begin(); i != meshes.end(); ++i)
//  {
//    mMeshComboBox->insertItem(listPosition, QString(i->first));
//    listPosition++;
//  }
//  //mMeshComboBox->blockSignals(false);
//}

void MeshPropertiesWidget::meshSelectedSlot(const QString& comboBoxText)
{
  mMeshPropertiesGroupBox->setEnabled(false);

  if (mMesh == mSelectMeshWidget->getMesh())
    return;
  
  mMesh = mSelectMeshWidget->getMesh();
  mMeshPropertiesGroupBox->setEnabled(mMesh!=0);

//  if(comboBoxText.isEmpty() || comboBoxText.endsWith("...")
//     || comboBoxText.endsWith(">"))
//  {
//    mMeshPropertiesGroupBox->setEnabled(false);
//    // Create empty current mesh
//    mMesh.reset();
//    return;
//  }
//  mMeshPropertiesGroupBox->setEnabled(true);
  if (!mMesh)
    return;
  
//  QString meshId = comboBoxText;
  
//  //find the mesh
//  ssc::MeshPtr mesh = ssc::dataManager()->getMesh(meshId);
//  if(!mesh)
//  {
//    ssc::messageManager()->sendError("Could not find the selected mesh in the DataManager: "+meshId);
//    return;
//  }
//
//  if(mMesh == mesh)
//    return;
  
  //Set new current image
//  mMesh = mesh;
  mParentFrameAdapter->setData(mMesh);
  mNameAdapter->setData(mMesh);
  mUidAdapter->setData(mMesh);
}
  
}//end namespace cx
