#include "cxImportDataWizard.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include "sscLabeledComboBoxWidget.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscData.h"

namespace cx
{

/***

TODO:
 - first copy failed can happen all the time - handle it.
 - select name combo
 - sync transform button

 */

ImportDataWizard::ImportDataWizard(ssc::DataPtr data, QWidget* parent) :
    QDialog(parent)
{
//  QWidget *firstPageWidget = new QFileDialog(this);
//  QWidget *secondPageWidget = new QWidget(this);
//
//  QStackedWidget *stackedWidget = new QStackedWidget;
//  stackedWidget->addWidget(firstPageWidget);
//  stackedWidget->addWidget(secondPageWidget);
  this->setWindowTitle("Set properties for imported data " + qstring_cast(data->getUid()));

  QVBoxLayout* layout = new QVBoxLayout(this);
//  layout->addWidget(stackedWidget);

  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
  mParentFrameAdapter->setData(data);
  ssc::LabeledComboBoxWidget*  combo = new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter);
  layout->addWidget(combo);

  QHBoxLayout* buttons = new QHBoxLayout(this);
  layout->addLayout(buttons);
  QPushButton* okButton = new QPushButton("OK", this);
  buttons->addStretch();
  buttons->addWidget(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  okButton->setDefault(true);


  ssc::messageManager()->sendInfo("Importing data...");
//  QString fileName = QFileDialog::getOpenFileName( this,
//                                  QString(tr("Select data file for import")),
//                                  mSettings->value("globalPatientDataFolder").toString(),
//                                  tr("Image/Mesh (*.mhd *.mha *.stl *.vtk)"));
//  if(fileName.isEmpty())
//  {
//    ssc::messageManager()->sendInfo("Import canceled");
//    return;
//  }
//
//  stateManager()->getPatientData()->importData(fileName);

}

ImportDataWizard::~ImportDataWizard()
{}

}//namespace cx
