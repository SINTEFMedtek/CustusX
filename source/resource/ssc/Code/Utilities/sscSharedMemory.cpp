// Note that if the server crashes, the system will leak memory.

#include "sscSharedMemory.h"

namespace ssc
{

// Shared header kept first in shared memory area
struct shm_header
{
	qint32 lastDone;	// index to last buffer that was written
	qint32 writeBuffer; // index to the currently held write buffer (-1 if no buffer is held)
	qint32 numBuffers;	// number of buffers
	qint32 bufferSize;	// size of each buffer
	qint32 headerSize;	// size of this header
	qint64 timestamp;	// timestamp of last buffer that was written
	qint32 buffer[0];	// number of readers currently operating on each buffer
};

SharedMemoryServer::SharedMemoryServer(QString key, int buffers, int sizeEach, QObject *parent) : mBuffer(key, parent)
{
	int headerSize = sizeof(struct shm_header) + buffers * sizeof(qint32);
	mSize = sizeEach;
	mBuffers = buffers;
	int size = buffers * sizeEach + headerSize;
	if (!mBuffer.create(size))
	{
		if (mBuffer.error() == QSharedMemory::AlreadyExists)
		{
			qWarning("Reusing existing buffer -- this should generally not happen");
			// reuse and overwrite; hopefully it was made by previous run of same program that crashed
			mBuffer.attach();
		}
		else
		{
			qWarning("Failed to create shared memory buffer of size %d: %s",
			         size, mBuffer.errorString().toAscii().constData());
			return;
		}
	}
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	header->bufferSize = mSize;
	header->numBuffers = mBuffers;
	header->headerSize = headerSize;
	header->lastDone = -1;
	header->writeBuffer = -1;
	header->timestamp = 0;
	memset(header->buffer, 0, sizeof(qint32) * buffers);
	mCurrentBuffer = -1;
}

SharedMemoryServer::~SharedMemoryServer()
{
}

// Find and return an available write buffer from the circle
void *SharedMemoryServer::buffer()
{
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	bool found = false;
	if (header)
	{
		mBuffer.lock();
		// Find first next buffer that is not being read; searching from left to right
		for (int i = mCurrentBuffer + 1; i < header->numBuffers && !found; i++)
		{
			if (header->buffer[i] == 0) // no read locks
			{
				found = true;
				internalRelease(false);
				mCurrentBuffer = i;
			}
		}
		for (int i = 0; i < header->numBuffers && !found; i++)
		{
			if (header->buffer[i] == 0) // no read locks
			{
				found = true;
				internalRelease(false);
				mCurrentBuffer = i;
			}
		}
		if (!found)
		{
			qWarning("Could not find an available write buffer");
			mBuffer.unlock();
			return NULL;
		}
		void *ptr = ((char *)header) + header->headerSize + header->bufferSize * mCurrentBuffer;
		header->writeBuffer = mCurrentBuffer;
		mBuffer.unlock();
		return ptr;
	}
	return NULL;
}

// Set last finished buffer to current write buffer, then unset current write buffer index.
// Note that timestamp is only set here, since this is the only place where it can be set 
// precisely.
void SharedMemoryServer::internalRelease(bool lock)
{
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	if (header && mCurrentBuffer >= 0)
	{
		if (lock) mBuffer.lock();
		header->lastDone = mCurrentBuffer;
		header->writeBuffer = -1;
		mLastTimestamp = QDateTime::currentDateTime();
		header->timestamp = mLastTimestamp.toMSecsSinceEpoch();
		if (lock) mBuffer.unlock();
		mCurrentBuffer = -1;
	}
}

void SharedMemoryServer::release()
{
	internalRelease(true);
}

SharedMemoryClient::SharedMemoryClient(QObject *parent) : mBuffer(parent)
{
	mSize = 0;
	mBuffers = 0;
	mCurrentBuffer = -1;
}

bool SharedMemoryClient::attach(const QString &key)
{
	mBuffer.setKey(key);
	bool success = mBuffer.attach(QSharedMemory::ReadWrite);
	if (success)
	{
		const struct shm_header *header = (const struct shm_header *)mBuffer.data();
		mSize = header->bufferSize;
		mBuffers = header->numBuffers;
	}
	return success;
}

bool SharedMemoryClient::detach()
{
	return mBuffer.detach();
}

const void *SharedMemoryClient::buffer(bool onlyNew)
{
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	if (header)
	{
		mBuffer.lock();
		if (header->lastDone == -1 || header->lastDone == header->writeBuffer ||
			( onlyNew && header->lastDone == mCurrentBuffer) )
		{
			mBuffer.unlock();
			return NULL; // Nothing 
		}
		if (mCurrentBuffer >= 0 && header->buffer[mCurrentBuffer] > 0)
		{
			header->buffer[mCurrentBuffer]--; // Release previous read lock
		}
		header->buffer[header->lastDone]++; // Lock new page against writing
		mCurrentBuffer = header->lastDone;
		const void *ptr = ((const char *)header) + header->headerSize + header->bufferSize * header->lastDone;
		mTimestamp.setMSecsSinceEpoch(header->timestamp);
		mBuffer.unlock();
		return ptr;
	}
	return NULL;
}

const void *SharedMemoryClient::isNew()
{
	return buffer(true);
}

void SharedMemoryClient::release()
{
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	if (header && mCurrentBuffer >= 0)
	{
		mBuffer.lock();
		if (header->buffer[mCurrentBuffer] > 0)
		{
			header->buffer[mCurrentBuffer]--;
		}
		mBuffer.unlock();
		mCurrentBuffer = -1;
	}
}

SharedMemoryClient::~SharedMemoryClient()
{
	release();
}

}
