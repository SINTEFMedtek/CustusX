/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
