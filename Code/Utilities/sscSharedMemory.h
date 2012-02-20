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

/**\brief Shared Memory Server
 *
 * Implements a circular buffer in shared memory using a client-server model,
 * where you have one writer and potentially multiple readers. Note that if you
 * have more readers than buffers, you can end up with all buffers busy when
 * you want to write new data. Readers always grab the latest buffer. Things go
 * more smooth when all users release their buffers as soon as they are done.
 *
 * \sa SharedMemoryClient
 * \ingroup sscUtility
 */
class SharedMemoryServer
{
private:
	QSharedMemory mBuffer;
	int mSize;
	int mBuffers;
	int mCurrentBuffer;
	QDateTime mLastTimestamp;

public:
	/**
	 * Constructor.
	 * \param key A string identifying this resource. Must be unique system wide
	 * \param buffers The number of buffers to allocate. This should be set to the maximum number of concurrent readers
	                  to this resource + 2, allowing one buffer for the writer (this class), one for each reader and a
					  spare so there is always a buffer for the writer to switch to.
	 * \param sizeEach The size of each buffer.
	 * \param parent The Qt parent object
	 */
	SharedMemoryServer(QString key, int buffers, int sizeEach, QObject *parent = 0);
	~SharedMemoryServer();
	int size() { return mSize; }
	int buffers() { return mBuffers; }
	QString key() { return mBuffer.key(); }
	void *buffer();		///< Grab and lock a write buffer
	void release();		///< Release our write buffer. Buffer will not be used before it is released.
	/**
	 * Return the timestamp of the last buffer written to
	 */
	QDateTime lastTimestamp() { return mLastTimestamp; }
	/**
	 * Return true if a write buffer is currently held, false otherwise
	 */
	bool hasBuffer() { return mCurrentBuffer != -1; }
private:
	void internalRelease(bool lock);
};

/**\brief Shared Memory Client
 *
 *
 * \sa SharedMemoryServer
 * \ingroup sscUtility
 */
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
	bool detach();
	int size() { return mSize; }
	int buffers() { return mBuffers; }
	QString key() { return mBuffer.key(); }
	const void *buffer(bool onlyNew=false);		///< Grab and lock a read buffer
	void release();			///< Release our read buffer
	const void *isNew();		///< Return new buffer only if new is available, otherwise return NULL
	QDateTime timestamp() { return mTimestamp; }
};

}

#endif
