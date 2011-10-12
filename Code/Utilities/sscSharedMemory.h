#ifndef SSC_SHARED_MEMORY_H
#define SSC_SHARED_MEMORY_H

#include <QSharedMemory>
#include <QDateTime>

namespace ssc
{

/// Implements a circular buffer in shared memory using a client-server model,
/// where you have one writer and potentially multiple readers. Note that if you
/// have more readers than buffers, you can end up with all buffers busy when
/// you want to write new data. Readers always grab the latest buffer. Things go
/// more smooth when all users release their buffers as soon as they are done.

class SharedMemoryServer
{
private:
	QSharedMemory mBuffer;
	int mSize;
	int mBuffers;
	int mCurrentBuffer;

public:
	SharedMemoryServer(QString key, int buffers, int sizeEach, QObject *parent = 0);
	~SharedMemoryServer();
	int size() { return mSize; }
	int buffers() { return mBuffers; }
	QString key() { return mBuffer.key(); }
	void *buffer();		///< Grab and lock a write buffer
	void release();		///< Release our write buffer. Buffer will not be used before it is released.
};

class SharedMemoryClient
{
private:
	QSharedMemory mBuffer;
	int mSize;
	int mBuffers;
	int mCurrentBuffer;
	QDateTime mTimestamp; ///< Time of writing of current buffer

public:
	SharedMemoryClient(QObject *parent = 0);
	~SharedMemoryClient();
	bool attach(const QString &key);
	int size() { return mSize; }
	int buffers() { return mBuffers; }
	QString key() { return mBuffer.key(); }
	const void *buffer();		///< Grab and lock a read buffer
	void release();			///< Release our read buffer
	const void *isNew();		///< Return new buffer only if new is available, otherwise return NULL
	QDateTime timestamp() { return mTimestamp; }
};

}

#endif
