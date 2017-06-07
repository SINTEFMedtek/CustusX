#include "cxFileReaderWriterService.h"
#include "cxFileReaderWriterServiceNull.h"
#include "cxNullDeleter.h"
#include <QFileInfo>

namespace cx
{

FileReaderWriterServicePtr FileReaderWriterService::getNullObject()
{
	static FileReaderWriterServicePtr mNull;
	if (!mNull)
		mNull.reset(new FileReaderWriterServiceNull, null_deleter());
	return mNull;
}

//----------------------------

bool sameSuffix(QString suffix1, QString suffix2)
{
	return (suffix1.compare(suffix2, Qt::CaseInsensitive) == 0 || suffix1.compare(suffix2, Qt::CaseInsensitive) == 0);
}

FileReaderWriterImplService::FileReaderWriterImplService(QString name, QString canReadDataType, QString canWriteDataType, QString fileSuffix) :
	mName(name),
	mCanReadDataType(canReadDataType),
	mCanWriteDataType(canWriteDataType),
	mFileSuffix(fileSuffix)
{
	this->setObjectName(mName);
}

QString FileReaderWriterImplService::getName() const
{
	return mName;
}

QString FileReaderWriterImplService::getFileSuffix() const
{
	return mFileSuffix;
}

bool FileReaderWriterImplService::canReadInternal(const QString &type, const QString &filename) const
{
	QString fileType = QFileInfo(filename).suffix();
	bool rightFileType = sameSuffix(fileType, mFileSuffix);
	bool rightDataType = (type == mCanReadDataType);
	return rightFileType && rightDataType;
}

bool FileReaderWriterImplService::canWriteInternal(const QString &type, const QString &filename) const
{
	QString fileType = QFileInfo(filename).suffix();
	bool rightFileType = sameSuffix(fileType, mFileSuffix);
	bool rightDataType = (type == mCanWriteDataType);
	return rightFileType && rightDataType;
}

}
