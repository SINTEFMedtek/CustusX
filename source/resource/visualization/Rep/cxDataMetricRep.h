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


#ifndef CXDATAMETRICREP_H_
#define CXDATAMETRICREP_H_

#include "cxRepImpl.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class CaptionText3D> CaptionText3DPtr;
typedef boost::shared_ptr<class DataMetric> DataMetricPtr;

/**
 * \brief Base class for all Data Metric reps.
 *
 * \ingroup cx_resource_visualization

 * \ingroup cx_resource_visualization_rep3D
 *
 * Handles common functionality: labels, size.
 *
 * \date Jul 31, 2011
 * \author Christian Askeland, SINTEF
 */
class DataMetricRep: public RepImpl
{
Q_OBJECT
public:
	virtual ~DataMetricRep() {}

	void setGraphicsSize(double size);
	void setLabelSize(double size);
	void setShowLabel(bool on);
	void setShowAnnotation(bool on);

    void setDataMetric(DataMetricPtr value);
    DataMetricPtr getDataMetric();

protected:
	DataMetricRep(const QString& uid, const QString& name);

    void addRepActorsToViewRenderer(View *view);
    void removeRepActorsFromViewRenderer(View *view);
    void drawText();
    virtual void clear(); // reset all internals
    virtual QString getText();

	double mGraphicsSize;
	bool mShowLabel;
	double mLabelSize;
	bool mShowAnnotation;
	Vector3D getColorAsVector3D() const;

    DataMetricPtr mMetric;
//    View *mView;

private:
	CaptionText3DPtr mText;
};
typedef boost::shared_ptr<class DataMetricRep> DataMetricRepPtr;

}

#endif /* CXDATAMETRICREP_H_ */
