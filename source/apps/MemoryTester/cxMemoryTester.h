/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXMEMORYTESTER_H_
#define CXMEMORYTESTER_H_

#include <QtGui>
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
