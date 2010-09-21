#include "sscMainWindow.h"
#include "sscDataManager.h"
#include "sscAxesRep.h"//test
#include "sscXmlOptionItem.h"
#include "sscTypeConversions.h"

  
MainWindow::MainWindow(QWidget* parent)
{
  this->setWindowTitle("US reconstruct test app");
  QHBoxLayout* layout = new QHBoxLayout(this);
  std::cout << qApp->applicationFilePath() << std::endl;

  ssc::XmlOptionFile settings(qApp->applicationDirPath()+"/usReconstruct.xml", "usReconstruction");

  mReconstructionWidget = new ssc::ReconstructionWidget(this, settings, "");
  
#define CA_DEFS
#ifdef CA_DEFS
//  QString defPath = "/Users/christiana/workspace/sessions/us_acq_holger_data/";
//  QString defFile = "ultrasoundSample5.mhd";
  QString defPath = "/Users/christiana/workspace/data/coordinateSys_test/";
  QString defFile = "USAcq_29.mhd";
#else
  //QString defPath = "/Users/olevs/data/UL_thunder/test/1/";
  //QString defFile = "UsAcq_1.mhd";
  
  QString defPath = "/Users/olevs/data/UL_thunder/test/coordinateSys_test/";
  //QString defPath = "/Users/olevs/data/UL_thunder/test/";
  QString defFile = "USAcq_29.mhd";
  //QSettings* settings = new QSettings();
  //defPath = settings->value("globalPatientDataFolder").toString();
  //defFile = "";
#endif
  
  mReconstructionWidget->selectData(defPath+defFile);
  
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
  if (!mVolumeRep)
  {
    mVolumeRep = ssc::VolumetricRep::New("test","test");
    mView->addRep(mVolumeRep);
  }

  //ssc::ImagePtr data = mReconstructionWidget->reconstructer()->getInput();
  ssc::ImagePtr data = mReconstructionWidget->reconstructer()->getOutput();
  mVolumeRep->setImage(data);

  //Test: show axes
  if (!mAxesRep)
  {
    mAxesRep = ssc::AxesRep::New("AxesRepUID");
    mView->addRep(mAxesRep);
  }

  // desperate attempt to show data automatically...
  mView->getRenderer()->Render();
  mView->getRenderer()->ResetCamera();
  mView->getRenderer()->Render();
  std::cout << "finished rendering volume " << data->getName() << std::endl;
}

