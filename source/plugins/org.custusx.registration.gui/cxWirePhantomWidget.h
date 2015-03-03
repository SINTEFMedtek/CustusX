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

#ifndef CXWIREPHANTOMWIDGET_H_
#define CXWIREPHANTOMWIDGET_H_

#include "org_custusx_registration_gui_Export.h"
#include "cxRegistrationBaseWidget.h"
#include "cxPipeline.h"
#include "cxFilter.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"

class QPushButton;
class QVBoxLayout;
class QTextEdit;

namespace cx
{
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;

/**
 * \brief Probe accuracy measurements using the Wire Phantom.
 * \ingroup cx_module_registration
 *
 *  \date Jun 21, 2012
 *  \date Nov 28, 2012
 *  \author christiana
 */
class org_custusx_registration_gui_EXPORT WirePhantomWidget: public RegistrationBaseWidget
{
Q_OBJECT

public:
	WirePhantomWidget(ctkPluginContext *pluginContext, QWidget* parent = 0);
	virtual ~WirePhantomWidget();
	virtual QString defaultWhatsThis() const;

protected:
	QVBoxLayout* mLayout;

private slots:
	void measureSlot();
	MeshPtr loadNominalCross();
	void registration();
	void generate_sMt();

private:
    void showDataMetrics(Vector3D cross_r);
    std::pair<QString, Transform3D> getLastProbePosition();
	void showData(DataPtr data);
	Vector3D findCentroid(MeshPtr mesh);

    class PipelineWidget* mPipelineWidget;
    PipelinePtr mPipeline;
	QPushButton* mMeasureButton;
	QPushButton* mCalibrationButton;
	QTextEdit* mResults;
	Transform3D mLastRegistration;
	UsReconstructionServicePtr mUsReconstructionService;
};

} /* namespace cx */
#endif /* CXWIREPHANTOMWIDGET_H_ */
