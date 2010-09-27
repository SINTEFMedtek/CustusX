#include "cxImportDataWizard.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <Qt>
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscData.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
namespace cx
{

/***

TODO:
 - first copy failed can happen all the time - handle it.
 - select name combo
 - sync transform button

 */

ImportDataWizard::ImportDataWizard(ssc::DataPtr data, QWidget* parent) :
    QDialog(parent),
    mData(data)
{
  this->setAttribute(Qt::WA_DeleteOnClose);

  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setWindowTitle("Set properties for imported data");

  layout->addWidget(new QLabel("Data uid:  " + qstring_cast(data->getUid())));
  layout->addWidget(new QLabel("Data name: " + qstring_cast(data->getName())));

  this->setInitialGuessForParentFrame();

  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
  mParentFrameAdapter->setData(data);
  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
  layout->addWidget(combo);

  QPushButton* importTransformButton = new QPushButton("Import Transform from Parent", this);
  importTransformButton->setToolTip("Replace data transform with that of the parent data.");
  connect(importTransformButton, SIGNAL(clicked()), this, SLOT(importTransformSlot()));
  layout->addWidget(importTransformButton);

  QHBoxLayout* buttons = new QHBoxLayout(this);
  layout->addLayout(buttons);
  QPushButton* okButton = new QPushButton("OK", this);
  buttons->addStretch();
  buttons->addWidget(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  okButton->setDefault(true);

  ssc::messageManager()->sendInfo("Importing data...");
}

ImportDataWizard::~ImportDataWizard()
{
  std::cout << "pang" << std::endl;
}

/** Use heuristics to guess a parent frame,
 *  based on similarities in name.
 */
void ImportDataWizard::setInitialGuessForParentFrame()
{
  if(!mData)
    return;

  QString base = qstring_cast(mData->getName()).split(".")[0];

  std::map<std::string, ssc::DataPtr> all = ssc::dataManager()->getData();
  for (std::map<std::string, ssc::DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
  {
    if (iter->second==mData)
      continue;
    QString current = qstring_cast(iter->second->getName());
    if (current.indexOf(base)>=0)
    {
      mData->setParentFrame(iter->first);
      break;
    }
  }

}

void ImportDataWizard::importTransformSlot()
{
  if(!mData)
    return;
  //ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  ssc::DataPtr parent = ssc::dataManager()->getData(mData->getParentFrame());
  if (!parent)
    return;
  mData->get_rMd_History()->setRegistration(parent->get_rMd());
  ssc::messageManager()->sendInfo("Assigned rMd from data [" + parent->getName() + "] to data [" + mData->getName() + "]");
}

}//namespace cx
