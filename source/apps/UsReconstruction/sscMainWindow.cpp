#include "sscMainWindow.h"
#include "sscDataManager.h"
  
MainWindow::MainWindow(QWidget* parent)
{
  this->setWindowTitle("US reconstruct test app");
  QHBoxLayout* layout = new QHBoxLayout(this);
  mReconstructionWidget = new ssc::ReconstructionWidget(this);
  layout->addWidget(mReconstructionWidget);

  mView = new ssc::View(this);
  layout->addWidget(mView, 1);

  connect(ssc::DataManager::getInstance(), SIGNAL(dataLoaded()), this, SLOT(showData()));
}

void MainWindow::automaticStart()
{
  mReconstructionWidget->reconstruct();
}

void MainWindow::showData()
{
  ssc::ImagePtr data = mReconstructionWidget->reconstructer()->getOutput();
  ssc::VolumetricRepPtr volumeRep = ssc::VolumetricRep::New("test","test");
  volumeRep->setImage(data);
  mView->addRep(volumeRep);

  // desperate attempt to show data automatically...
  mView->getRenderer()->Render();
  mView->getRenderer()->ResetCamera();
  mView->getRenderer()->Render();
  std::cout << "finished rendering volume " << data->getName() << std::endl;
}

