#include "cxMemoryTester.h"
#include <vtkImageData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>

namespace cx
{

vtkImageDataPtr generateVtkImageData()
{
	int dimension = 100; // generate 100^3 = 1Mb volume.

	vtkImageDataPtr data = vtkImageDataPtr::New();
	data->SetSpacing(1, 1, 1);
	data->SetExtent(0, dimension-1, 0, dimension-1, 0, dimension-1);
	data->SetScalarTypeToUnsignedChar();
	data->SetNumberOfScalarComponents(1);

	int scalarSize = dimension*dimension*dimension;

	unsigned char *rawchars = (unsigned char*)malloc(scalarSize+1);
	char initValue = 1;
	std::fill(rawchars,rawchars+scalarSize, initValue);

	vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
	array->SetNumberOfComponents(1);
	//TODO: Whithout the +1 the volume is black
	array->SetArray(rawchars, scalarSize+1, 0); // take ownership
	data->GetPointData()->SetScalars(array);

	// A trick to get a full LUT in ssc::Image (automatic LUT generation)
	// Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
	rawchars[0] = 255;
	data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
	rawchars[0] = 0;

	return data;
}

int MemHolder::addBlock()
{
	Block block;
	int N = 100;
	for (unsigned i=0; i<N; ++i)
	{
		block.mData.push_back(generateVtkImageData());
	}

	mBlocks.push_back(block);
	std::cout << QString("generated memory: %1 Mb, %2 blocks").arg(double(block.mData.front()->GetActualMemorySize() * N) / 1000.0).arg(mBlocks.size()).toStdString() << std::endl;

}

int MemHolder::removeBlock()
{
	mBlocks.pop_back();
	std::cout << QString("removed one block, %1 left").arg(mBlocks.size()).toStdString() << std::endl;
}





MemoryTester::MemoryTester(QWidget* parent) : QMainWindow(parent)
{
	mMemory.reset(new MemHolder);

	this->setWindowTitle("QtSandbox");

//    mTextEdit = new QPlainTextEdit;
//    setCentralWidget(mTextEdit);
	this->setCentralWidget(new QPlainTextEdit);

    addActions();
    addToolbar();
    addMenu();
    // play with this one:
    //setUnifiedTitleAndToolBarOnMac(true);

    createStatusBar();

//	cx::LayoutEditor* editor = new cx::LayoutEditor(this);
//    this->setCentralWidget(editor);
}

void MemoryTester::addActions()
{
	mAction1 = new QAction("Action1", this);
	mAction1->setIcon(QIcon(":/images/go-home.png"));
//    mAction1->setIcon(QIcon(":/images/workflow_state_navigation.png"));

	mAddMemAction = new QAction("AddMem", this);
	mRemoveMemAction = new QAction("RemMem", this);

	connect(mAddMemAction, SIGNAL(triggered()), mMemory.get(), SLOT(addBlock()));
	connect(mRemoveMemAction, SIGNAL(triggered()), mMemory.get(), SLOT(removeBlock()));

//	mAction2 = new QAction("Action2", this);
////    mAction2->setIcon(QIcon(":/images/application-exit.png"));
//    mAction2->setIcon(QIcon(":/images/workflow_state_navigation2.png"));

//  mAction3 = new QAction("Action3", this);
//  mAction3->setIcon(QIcon(":/images/workflow_state_patient_data.png"));

//  mAction4 = new QAction("Action4", this);
//  mAction4->setIcon(QIcon(":/images/workflow_state_patient_data2.png"));

      mAboutQtAct = new QAction(tr("About &Qt"), this);
    mAboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(mAboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    mAboutAct = new QAction(tr("About QtSandbox"), this);
    mAboutAct->setStatusTip(tr("Show the Application's About box"));
    connect(mAboutAct, SIGNAL(triggered()), this, SLOT(about()));

    mCrashAct = new QAction(tr("Color crash"), this);
    connect(mCrashAct, SIGNAL(triggered()), this, SLOT(colorCrash()));
}

void MemoryTester::addToolbar()
{
	mToolbar = this->addToolBar("Mytoolbar");
	mToolbar->addAction(mAction1);
	mToolbar->addAction(mAddMemAction);
  mToolbar->addAction(mRemoveMemAction);
//	mToolbar->addAction(mCrashAct);
}

void MemoryTester::addMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(mAction1);
    fileMenu->addAction(mCrashAct);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(mAboutQtAct);
    helpMenu->addAction(mAboutAct);
}


void MemoryTester::about()
{
   QMessageBox::about(this, tr("About QtSandbox"),
            tr("The <b>QtSandbox</b> is a place where you can "
               "test out nifty Qt features in a small app. Enjoy!"));
}

void MemoryTester::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MemoryTester::colorCrash()
{
#if QT_VERSION >= 0x040500
   QColorDialog dialog(QColor("white"), this);
  dialog.exec();
#else
  QColor result = QColorDialog::getColor(QColor("white"), this);
#endif
}
}
