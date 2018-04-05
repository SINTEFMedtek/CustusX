/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
