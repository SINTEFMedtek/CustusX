/*
 * cxImageSenderFactory.h
 *
 *  \date Aug 9, 2011
 *      \author christiana
 */

#ifndef CXIMAGESENDERFACTORY_H_
#define CXIMAGESENDERFACTORY_H_

#include <QObject>
#include <QStringList>
#include <map>
class QTcpSocket;

namespace cx
{

typedef std::map<QString, QString> StringMap;
StringMap extractCommandlineOptions(QStringList cmdline);
int convertStringWithDefault(QString text, int def);

/**Factory class for creation of MHDImageSender objects.
 * MHDImageSender objects have no common base except QObject,
 * but have the following property:
 * When created, start grabbing and emitting images using
 * OpenIGTLink on the provided socket.
 */
class ImageSenderFactory
{
public:
	QStringList getSenderTypes() const; ///< all available MHDImageSender types
	QStringList getArgumentDescription(QString type) const; ///< arguments for one Image Sender
	QObject* createSender(QString type, QTcpSocket* socket, StringMap arguments) const; ///< launch a MHDImageSender.
};

}

#endif /* CXIMAGESENDERFACTORY_H_ */
