#ifndef CXFILEWATCHERWIDGET_H_
#define CXFILEWATCHERWIDGET_H_

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"

class QFileSystemWatcher;
class QFile;

namespace cx
{

/**
 * \class FileWatcherWidget
 *
 * \brief Baseclass for widgets that should watch a file.
 *
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class FileWatcherWidget : public BaseWidget
{
	Q_OBJECT

public:
	FileWatcherWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~FileWatcherWidget();

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath) = 0;

protected:
	bool internalOpenNewFile(const QString absoluteFilePath);
	void watchFile(bool on);

	QFileSystemWatcher*				mFileSystemWatcher;
	boost::shared_ptr<QFile>	mCurrentFile;
};

} /* namespace cx */
#endif /* CXFILEWATCHERWIDGET_H_ */
