/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOPENIGTLINKTOOL_H
#define CXOPENIGTLINKTOOL_H

#include "org_custusx_core_openigtlink3_Export.h"

#include "cxToolImpl.h"

#include <QTimer>
#include <boost/shared_ptr.hpp>

#include "igtlioBaseConverter.h"

#include "cxTransform3D.h"
#include "cxToolConfigurationParser.h"

class QStringList;

namespace cx
{
typedef std::vector<double> DoubleVector;
typedef boost::shared_ptr<DoubleVector> DoubleVectorPtr;
typedef std::vector<Transform3DPtr> Transform3DVector;
typedef boost::shared_ptr<Transform3DVector> Transform3DVectorPtr;
typedef boost::shared_ptr<class IgstkTool> IgstkToolPtr;
typedef boost::shared_ptr<class ProbeImpl> ProbeImplPtr;

/**
 * \file
 * \addtogroup org_custusx_core_openigtlink
 * @{
 */


/**
 * \brief Class representing the tools a navigation system can recognize.
 * \ingroup org_custusx_core_tracking
 *
 * \date 03 March 2015
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkTool: public ToolImpl
{
    friend class OpenIGTLinkTrackingSystemService;
    Q_OBJECT

public:
	OpenIGTLinkTool(ConfigurationFileParser::ToolStructure configFileToolStructure, ToolFileParser::ToolInternalStructurePtr toolFileToolStructure);
    virtual ~OpenIGTLinkTool();

    virtual bool getVisible() const;
    virtual bool isInitialized() const;
    virtual QString getUid() const;
    virtual QString getName() const;
    virtual ProbePtr getProbe() const;
    virtual double getTimestamp() const;
    virtual double getTooltipOffset() const; ///< get a virtual offset extending from the tool tip.
    virtual void setTooltipOffset(double val); ///< set a virtual offset extending from the tool tip.

    virtual bool isCalibrated() const; ///< true if calibration is different from identity
    virtual Transform3D getCalibration_sMt() const; ///< get the calibration transform from tool space to sensor space (where the spheres or similar live)
    virtual void setCalibration_sMt(Transform3D sMt); ///< requests to use the calibration and replaces the tools calibration file

    //virtual void set_prMt(const Transform3D& prMt, double timestamp);
    virtual void setVisible(bool vis);

	bool doIdCorrespondToTool(QString openIGTLinkId);
	bool isReference();
protected:
	virtual ToolFileParser::ToolInternalStructurePtr getToolFileToolStructure() const;
private slots:
    void toolTransformAndTimestampSlot(Transform3D prMs, double timestamp); ///< timestamp is in milliseconds
    void calculateTpsSlot();
    void toolVisibleSlot(bool);

private:
    ProbePtr mProbe;
    QTimer mTpsTimer;
    double mTimestamp;
		bool mVisible;
		qint64 mLastReceivedPositionTime;
		bool mPrintedWarningAboutTimeStampMismatch;

		//Store these structures directly for now
		ConfigurationFileParser::ToolStructure mConfigFileToolStructure;
		ToolFileParser::ToolInternalStructurePtr mToolFileToolStructure;

		void calculateVisible();
		void checkTimestampMismatch();
		void printWarningAboutTimestampMismatch(double diff);
};
typedef boost::shared_ptr<OpenIGTLinkTool> OpenIGTLinkToolPtr;

/**
 * @}
 */
} //namespace cx
#endif /* CXOPENIGTLINKTOOL_H */
