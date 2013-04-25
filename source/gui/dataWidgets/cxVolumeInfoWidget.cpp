#include "cxVolumeInfoWidget.h"

#include <QPushButton>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscLabeledLineEditWidget.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "sscVolumeHelpers.h"
#include "cxActiveImageProxy.h"

namespace cx
{

VolumeInfoWidget::VolumeInfoWidget(QWidget* parent) :
  BaseWidget(parent, "VolumeInfoWidget", "Volume Info")
{
  //layout
  QVBoxLayout* toptopLayout = new QVBoxLayout(this);

  QGridLayout* gridLayout = new QGridLayout;
  toptopLayout->addLayout(gridLayout);

  QPushButton* deleteButton = new QPushButton("Delete", this);
  deleteButton->setToolTip("Remove the selected Image from the system.");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteDataSlot()));

  mParentFrameAdapter = ParentFrameStringDataAdapter::New();
  mNameAdapter = DataNameEditableStringDataAdapter::New();
  mUidAdapter = DataUidEditableStringDataAdapter::New();
  mModalityAdapter = DataModalityStringDataAdapter::New();
  mImageTypeAdapter = ImageTypeStringDataAdapter::New();

  int i=0;
  gridLayout->addWidget(new ssc::LabeledLineEditWidget(this, mUidAdapter), i++, 0, 1, 2);
  new ssc::LabeledLineEditWidget(this, mNameAdapter, gridLayout, i++);
  new ssc::LabeledComboBoxWidget(this, mModalityAdapter, gridLayout, i++);
  new ssc::LabeledComboBoxWidget(this, mImageTypeAdapter, gridLayout, i++);
  new ssc::LabeledComboBoxWidget(this, mParentFrameAdapter, gridLayout, i++);
  mTextBrowser = new QTextBrowser();
  gridLayout->addWidget(mTextBrowser, i++, 0, 1, 2);

//  int i=0;
//  gridLayout->addWidget(uidEdit,        i++, 0);
//  gridLayout->addWidget(nameEdit,       i++, 0);
//  gridLayout->addWidget(modalityCombo,  i++, 0);
//  gridLayout->addWidget(imageTypeCombo, i++, 0);
//  gridLayout->addWidget(parentFrame,    i++, 0);
  gridLayout->addWidget(deleteButton, i++, 0, 1, 2);

  toptopLayout->addStretch();

  mActiveImageProxy = ActiveImageProxy::New();
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(updateSlot()));
  //TODO: Check if the following are needed
//  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateSlot()));
//  connect(mActiveImageProxy.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(updateSlot()));
  updateSlot();
}

VolumeInfoWidget::~VolumeInfoWidget()
{}

QString VolumeInfoWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Volume information</h3>"
      "<p>Displays information about a selected volume.</p>"
      "<p><i></i></p>"
      "</html>";
}

/*
void VolumeInfoWidget::deleteDataSlot()
{
  if (!ssc::dataManager()->getActiveImage())
    return;
  ssc::dataManager()->removeData(ssc::dataManager()->getActiveImage()->getUid());
}
*/

void VolumeInfoWidget::updateSlot()
{
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	mParentFrameAdapter->setData(image);
	mNameAdapter->setData(image);
	mUidAdapter->setData(image);
	mModalityAdapter->setData(image);
	mImageTypeAdapter->setData(image);

	mTextBrowser->clear();
	QString text = "";
	std::map<std::string, std::string> info = ssc::getDisplayFriendlyInfo(image);
	std::map<std::string, std::string>::iterator it;
	for(it = info.begin(); it != info.end(); ++it)
	{
		text += "<b>"+qstring_cast(it->first)+":</b> "+qstring_cast(it->second)+"<br>";
	}
	QTextDocument* doc = new QTextDocument();
	doc->setHtml(text);
	mTextBrowser->setDocument(doc);
}
}//namespace
