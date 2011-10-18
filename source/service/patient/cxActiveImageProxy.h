/*
 * cxActiveImageProxy.h
 *
 *  Created on: Oct 18, 2011
 *      Author: olevs
 */

#ifndef CXACTIVEIMAGEPROXY_H_
#define CXACTIVEIMAGEPROXY_H_

#include "sscImage.h"

namespace cx
{

typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;
/**
 * Helper class for connection the active image.
 * By listeing to this class, you will always listen
 * to the active tool.
 */
class ActiveImageProxy : public QObject
{
	Q_OBJECT
public:
	static ActiveImageProxyPtr New() { return ActiveImageProxyPtr(new ActiveImageProxy()); }
	ActiveImageProxy();

signals:
	void activeImageChanged(const QString& uid);///< The original image changed signal from DataManager

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

}

#endif /* CXACTIVEIMAGEPROXY_H_ */
