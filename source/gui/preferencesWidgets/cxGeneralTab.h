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

#ifndef CXGENERALTAB_H_
#define CXGENERALTAB_H_

#include "cxGuiExport.h"

#include <QComboBox>
#include "cxPreferenceTab.h"
#include "cxBoolProperty.h"
class QToolButton;

namespace cx
{

/**
 * \class GeneralTab
 *
 * \brief Tab for general settings in the system
 *
 * \date Jan 25, 2010
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 * \author Frank Lindseth, SINTEF
 */
class cxGui_EXPORT GeneralTab : public PreferenceTab
{
  Q_OBJECT

public:
  GeneralTab(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget *parent = 0);
  virtual ~GeneralTab();
  void init();

public slots:
  void saveParametersSlot();

private slots:
  void browsePatientDataFolderSlot();
  void browseVLCPathSlot();
  void onAddProfile();
	void onProfileSelected();
	void onProfileChanged();
	void selectProfile(QString uid);

private:
	void rejectDialog();
  void searchForVLC(QStringList searchPaths = QStringList());
  QToolButton* createAddProfileButton();
  StringPropertyPtr getProfileSelector();

  QComboBox* mPatientDataFolderComboBox;
  QComboBox* mVLCPathComboBox;

  QComboBox* mToolConfigFolderComboBox;

  BoolPropertyPtr mFilterToolPositions;
  DoublePropertyPtr mFilterToolPositionsCutoff;

  QString mGlobalPatientDataFolder;
  QString mVLCPath;
  ViewServicePtr mViewService;
  PatientModelServicePtr mPatientModelService;
  StringPropertyPtr mSelector;

};
} /* namespace cx */
#endif /* CXGENERALTAB_H_ */
