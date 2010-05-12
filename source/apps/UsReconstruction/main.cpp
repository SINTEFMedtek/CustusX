/*
 *  main.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include <QtGui>
#include <vtkRenderer.h>
#include "sscReconstructionWidget.h"
#include "sscView.h"
#include "sscVolumetricRep.h"
#include "sscImage.h"

/** Test app for UsReconstruction
 */
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
  
	QWidget* top = new QWidget();
	top->setWindowTitle("US reconstruct test app");
  QHBoxLayout* layout = new QHBoxLayout(top);
  ssc::ReconstructionWidget* reconstructionWidget = new ssc::ReconstructionWidget(top);
  layout->addWidget(reconstructionWidget);
  
  ssc::View* view = new ssc::View(top);
  layout->addWidget(view);
  
  ssc::ImagePtr data = reconstructionWidget->mReconstructer->getOutput();
  
  ssc::VolumetricRepPtr volumeRep = ssc::VolumetricRep::New("test","test");
  volumeRep->setImage(data);
  
  view->addRep(volumeRep);
  
  top->resize(700, 600);
  
  top->show();
#ifdef __MACOSX__ // needed on mac for bringing to front: does the opposite on linux
  top->activateWindow();
#endif
  top->raise();
  view->getRenderer()->ResetCamera();
     
	int val = app.exec();
	return val;
	//return 0;
}
