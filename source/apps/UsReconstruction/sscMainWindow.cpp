#include "sscMainWindow.h"
#include "sscDataManager.h"
#include "sscAxesRep.h"//test
#include "sscXmlOptionItem.h"
#include "sscTypeConversions.h"
#include "sscReconstructPreprocessor.h"

  
MainWindow::MainWindow(QWidget* parent)
{
  this->setWindowTitle("US reconstruct test app");
  QHBoxLayout* layout = new QHBoxLayout(this);
  std::cout << qApp->applicationFilePath() << std::endl;

  ssc::XmlOptionFile settings(qApp->applicationDirPath()+"/usReconstruct.xml", "usReconstruction");

  mReconstructionWidget = new cx::ReconstructionWidget(this, ssc::ReconstructManagerPtr(new ssc::ReconstructManager(settings, "")));
  
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

  mView = new ssc::ViewWidget(this);
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

  ssc::ReconstructPreprocessorPtr preprocessor = mReconstructionWidget->reconstructer()->createPreprocessor();
  std::vector<ssc::ReconstructCorePtr> cores = mReconstructionWidget->reconstructer()->createCores();
  preprocessor->initializeCores(cores);
  ssc::ImagePtr data = cores[0]->reconstruct();

  //ssc::ImagePtr data = mReconstructionWidget->reconstructer()->getInput();
  //ssc::ImagePtr data = mReconstructionWidget->reconstructer()->getReconstructer()->createCore()->reconstruct();
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

