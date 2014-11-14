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

#ifndef CXVISUALIZATIONSERVICE_H
#define CXVISUALIZATIONSERVICE_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "cxData.h"

#define VisualizationService_iid "cx::VisualizationService"

namespace cx
{

typedef boost::shared_ptr<class VisualizationService> VisualizationServicePtr;
typedef boost::shared_ptr<class ViewGroup> ViewGroupPtr;
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

/** \brief Visualization services
 *
 *  \ingroup cx_resource_visualization
 *  \date 2014-09-19
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT VisualizationService : public QObject
{
	Q_OBJECT
public:
	virtual ViewPtr get3DView(int group = 0, int index = 0) = 0;

	virtual int getActiveViewGroup() const = 0;
	virtual ViewGroupDataPtr getViewGroupData(int groupIdx) = 0;

	virtual void setRegistrationMode(REGISTRATION_STATUS mode) = 0;

	virtual void autoShowData(DataPtr data) = 0;
	virtual void enableRender(bool val) = 0;
	virtual bool renderingIsEnabled() const = 0;

	virtual bool isNull() = 0;
	static VisualizationServicePtr getNullObject();

signals:
	void renderingEnabledChanged();
	void activeViewChanged(); ///< emitted when the active view changes

public slots:

};

} //cx
Q_DECLARE_INTERFACE(cx::VisualizationService, VisualizationService_iid)

#endif // CXVISUALIZATIONSERVICE_H
