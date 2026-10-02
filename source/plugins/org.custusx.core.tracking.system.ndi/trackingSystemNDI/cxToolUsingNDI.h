/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLUSINGNNDI_H_
#define CXTOOLUSINGNNDI_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include "cxToolImpl.h"

#include <QTimer>
#include <boost/shared_ptr.hpp>
#include "cxTransform3D.h"
#include "cxDefinitions.h"

namespace cx
{
typedef boost::shared_ptr<class NdiTool> NdiToolPtr;
typedef boost::shared_ptr<class ProbeImpl> ProbeImplPtr;

/**
 * \file
 * \addtogroup org_custusx_core_tracking_system_ndi
 * @{
 */

/**
 * \brief CustusX Tool adapter for an NDI-tracked tool.
 * \ingroup org_custusx_core_tracking_system_ndi
 *
 * Wraps an NdiTool in the CustusX Tool interface (ToolImpl).
 * Adapts NDI transform signals to the CustusX tool position history and signals.
 */
class org_custusx_core_tracking_system_ndi_EXPORT ToolUsingNDI : public ToolImpl
{
	Q_OBJECT

public:
	explicit ToolUsingNDI(NdiToolPtr ndiTool);
	virtual ~ToolUsingNDI();

	virtual bool getVisible() const;
	virtual bool isInitialized() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual int getIndex() const { return 0; }
	virtual ProbePtr getProbe() const;
	virtual double getTimestamp() const { return mTimestamp; }
	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual bool isCalibrated() const;
	virtual Transform3D getCalibration_sMt() const;
	virtual void setCalibration_sMt(Transform3D calibration);

	bool isValid() const;

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

	virtual void addXml(QDomNode& dataNode);
	virtual void parseXml(QDomNode& dataNode);

signals:
	void attachedToTracker(bool);

protected:
	virtual ToolFileParser::ToolInternalStructurePtr getToolFileToolStructure() const;

private slots:
	void toolTransformAndTimestampSlot(Transform3D matrix, double timestamp, ToolPositionMetadata metadata);
	void toolVisibleSlot(bool visible);
	void calculateTpsSlot();

private:
	NdiToolPtr mTool;
	bool mValid;
	ProbePtr mProbe;
	QTimer mTpsTimer;
	double mTimestamp;
};
typedef boost::shared_ptr<ToolUsingNDI> ToolUsingNDIPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXTOOLUSINGNNDI_H_ */
