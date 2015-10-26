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

#ifndef CXACTIVEIMAGEPROXY_H_
#define CXACTIVEIMAGEPROXY_H_

#include "cxResourceExport.h"

#include <QObject>
#include "cxForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;
/**
 * \brief Helper class for connection the active image.
 * \ingroup cx_resource_core_data
 *
 * By listening to this class, you will always listen
 * to the active image.
 *
 *  \date Oct 18, 2011
 *  \author Ole Vegard Solberg, SINTEF
 *
 */
class cxResource_EXPORT ActiveImageProxy: public QObject
{
Q_OBJECT
public:
	static ActiveImageProxyPtr New(ActiveDataPtr activeData)
	{
		return ActiveImageProxyPtr(new ActiveImageProxy(activeData));
	}
	ActiveImageProxy(ActiveDataPtr activeData);
	~ActiveImageProxy();

signals:
	void activeImageChanged(const QString& uid); ///< The original image changed signal from DataManager

	// Forwarding active image signals
	void transformChanged();
	void propertiesChanged();
	void landmarkRemoved(QString uid);
	void landmarkAdded(QString uid);
	void vtkImageDataChanged();
	void transferFunctionsChanged();
	void clipPlanesChanged();
	void cropBoxChanged();

private slots:
	void activeImageChangedSlot(const QString&);
private:
	ImagePtr mImage;
	ActiveDataPtr mActiveData;
};

/**
 * @}
 */
}

#endif /* CXACTIVEIMAGEPROXY_H_ */
