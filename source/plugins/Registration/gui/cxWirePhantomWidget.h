// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXWIREPHANTOMWIDGET_H_
#define CXWIREPHANTOMWIDGET_H_

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


/**
 * \brief Probe accuracy measurements using the Wire Phantom.
 * \ingroup cx_plugin_registration
 *
 *  \date Jun 21, 2012
 *  \date Nov 28, 2012
 *  \author christiana
 */
class WirePhantomWidget: public RegistrationBaseWidget
{
Q_OBJECT

public:
	WirePhantomWidget(RegistrationManagerPtr regManager, QWidget* parent);
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

};

} /* namespace cx */
#endif /* CXWIREPHANTOMWIDGET_H_ */
