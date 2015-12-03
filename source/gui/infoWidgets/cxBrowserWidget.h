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
#ifndef CXBROWSERWIDGET_H_
#define CXBROWSERWIDGET_H_

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxPopupToolbarWidget.h"
#include "cxXmlOptionItem.h"
#include "cxControllableSplitter.h"
#include "cxReplacableContentWidget.h"

class QTreeWidget;
class QVBoxLayout;
class QTreeWidgetItem;
class QTreeView;
class QSplitter;
class QLineEdit;

namespace cx
{

class PopupToolbarWidget;
class TreeItemModel;
class EraseDataToolButton;
typedef boost::shared_ptr<class TreeRepository> TreeRepositoryPtr;


/**
 * \class BrowserWidget
 *
 * \brief Shows a treestructure containing the loaded images, meshes
 * and tools.
 * \ingroup cxGUI
 *
 * \date Feb 11, 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class BrowserWidget : public BaseWidget
{
  Q_OBJECT

public:
  BrowserWidget(QWidget* parent, VisServicesPtr services);
  ~BrowserWidget();

protected slots:
  void resetView(); // called when tree is reset

protected:
  TreeItemModel* mModel;
  QPointer<QTreeView> mTreeView;

protected:
	virtual void prePaintEvent();
  
private:
  void createGUI();
  void onPopup();
  void onCurrentItemChanged();
  void createButtonWidget(QWidget* widget);
  XmlOptionItem getShowToolbarOption();
  void expandDefault(QModelIndex index);
  void updateNodeName();
  void eraseCurrentNode();

  VisServicesPtr mServices;
  ControllableSplitter* mSplitter;
  QPointer<ReplacableContentWidget> mPropertiesWidget;
  PopupToolbarWidget* mPopupWidget;
  XmlOptionFile mOptions;
//  QLabel* mNameLabel;
  QLineEdit* mName;
  EraseDataToolButton* mRemoveButton;

  StringPropertyPtr mFilterSelector;
};
}//end namespace cx

#endif /* CXBROWSERWIDGET_H_ */
