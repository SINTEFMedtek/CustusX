/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMEMORYTESTER_H_
#define CXMEMORYTESTER_H_

#include <QtWidgets>

#include <vector>
#include "vtkForwardDeclarations.h"
#include <boost/shared_ptr.hpp>

namespace cx
{

class MemHolder : public QObject
{
	Q_OBJECT
public:
	struct Block
	{
		std::vector<vtkImageDataPtr> mData;
	};

	std::vector<Block> mBlocks;
public slots:
	int addBlock();
	int removeBlock();
	void generateLeak();
private:
	vtkImageDataPtr  generateVtkImageData();
};

class MemoryTester : public QMainWindow
{
	Q_OBJECT
public:
	MemoryTester(QWidget* parent = 0);
	~MemoryTester() {}
	QToolBar* mToolbar;
//	QPlainTextEdit* mTextEdit;
private slots:
	void about();
	void colorCrash();
private:
	void createStatusBar();
	void addActions();
	void addToolbar();
	void addMenu();

	QAction* mAddMemAction;
	QAction* mRemoveMemAction;
	QAction* mLeakAction;

	QAction* mAction1;
//	QAction* mAction2;
//  QAction* mAction3;
//  QAction* mAction4;
    QAction* mAboutQtAct;
    QAction* mAboutAct;

    QAction* mCrashAct;

	boost::shared_ptr<MemHolder> mMemory;
};


}  // namespace cx

#endif // CXMEMORYTESTER_H_
