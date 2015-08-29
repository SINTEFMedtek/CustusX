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

#ifndef CXOPENIGTLINKTOOL_H
#define CXOPENIGTLINKTOOL_H

#include "org_custusx_core_openigtlink_Export.h"

#include "cxToolImpl.h"

#include <QTimer>
#include <boost/shared_ptr.hpp>
#include "cxTransform3D.h"

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
class org_custusx_core_openigtlink_EXPORT OpenIGTLinkTool: public ToolImpl
{
    friend class OpenIGTLinkTrackingSystemService;
    Q_OBJECT

public:
    OpenIGTLinkTool(QString uid);
    virtual ~OpenIGTLinkTool();

    virtual std::set<Type> getTypes() const;

    virtual vtkPolyDataPtr getGraphicsPolyData() const;
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

private slots:
    void toolTransformAndTimestampSlot(Transform3D prMs, double timestamp); ///< timestamp is in milliseconds
    void calculateTpsSlot();
    void toolVisibleSlot(bool);

private:
    std::set<Type> determineTypesBasedOnUid(const QString uid) const;
    bool isProbe() const;
    void createPolyData();

    vtkPolyDataPtr mPolyData; ///< the polydata used to represent the tool graphically
    ProbePtr mProbe;
    QTimer mTpsTimer;
    double mTimestamp;
    std::set<Type> mTypes;
    Transform3D m_sMt_calibration;
};
typedef boost::shared_ptr<OpenIGTLinkTool> OpenIGTLinkToolPtr;

/**
 * @}
 */
} //namespace cx
#endif /* CXOPENIGTLINKTOOL_H */
