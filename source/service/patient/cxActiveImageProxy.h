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

#ifndef CXACTIVEIMAGEPROXY_H_
#define CXACTIVEIMAGEPROXY_H_

#include <QObject>
#include "sscForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;
/**
 * \brief Helper class for connection the active image.
 * \ingroup cxServicePatient
 *
 * By listening to this class, you will always listen
 * to the active tool.
 *
 *  \date Oct 18, 2011
 *  \author olevs
 *
 */
class ActiveImageProxy: public QObject
{
Q_OBJECT
public:
	static ActiveImageProxyPtr New()
	{
		return ActiveImageProxyPtr(new ActiveImageProxy());
	}
	ActiveImageProxy();

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
	ssc::ImagePtr mImage;
};

/**
 * @}
 */
}

#endif /* CXACTIVEIMAGEPROXY_H_ */
