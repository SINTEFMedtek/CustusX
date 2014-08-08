#ifndef CXIMAGESENDERFACTORY_H_
#define CXIMAGESENDERFACTORY_H_

#include <QObject>
#include <QStringList>
#include <map>
#include <vector>
#include "cxImageStreamer.h"

class QTcpSocket;

namespace cx
{

typedef std::map<QString, QString> StringMap;
StringMap extractCommandlineOptions(QStringList cmdline);


/**
 * \brief Factory class for creating streamer objects.
 *
 * \ingroup cx_resource_videoserver
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 * \date Aug 9, 2011
 */
class CommandlineImageStreamerFactory
{
public:
	CommandlineImageStreamerFactory();
	QString getDefaultSenderType() const;
	QStringList getSenderTypes() const; ///< all available sender types
	QStringList getArgumentDescription(QString type) const; ///< arguments for one streamer
	StreamerPtr getImageSender(QString type);
	StreamerPtr getFromArguments(StringMap args);

private:
	std::vector<CommandLineStreamerPtr> mCommandLineStreamers;
	std::vector<ImageStreamerPtr> mImageStreamers;
};

}

#endif /* CXIMAGESENDERFACTORY_H_ */
