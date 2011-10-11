// Note that if the server crashes, the system will leak memory.

#include "sscSharedMemory.h"

namespace ssc
{

#include <stdint.h>

// Shared header kept first in shared memory area
struct shm_header
{
	int32_t lastDone;	// index to last buffer that was written
	int32_t numBuffers;	// number of buffers
	int32_t	bufferSize;	// size of each buffer
	int32_t headerSize;	// size of this header
	int32_t	buffer[0];	// number of readers currently operating on each buffer
};

SharedMemoryServer::SharedMemoryServer(int buffers, int sizeEach, QObject *parent) : mBuffer(parent)
{
	int headerSize = sizeof(struct shm_header) + buffers * sizeof(int32_t);
	mSize = sizeEach;
	mBuffers = buffers;
	if (!mBuffer.create(buffers * sizeEach + headerSize))
	{
		qWarning("Failed to create shared memory buffer");
		return;
	}
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	header->bufferSize = mSize;
	header->numBuffers = mBuffers;
	header->lastDone = -1;
	header->headerSize = headerSize;
	memset(header->buffer, 0, sizeof(int32_t) * buffers);
	mCurrentBuffer = -1;
}

SharedMemoryServer::~SharedMemoryServer()
{
}

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
				header->lastDone = mCurrentBuffer;
				mCurrentBuffer = i;
			}
		}
		for (int i = 0; i < header->numBuffers && !found; i++)
		{
			if (header->buffer[i] == 0) // no read locks
			{
				found = true;
				header->lastDone = mCurrentBuffer;
				mCurrentBuffer = i;
			}
		}
		if (!found)
		{
			qWarning("Could not find an available write buffer");
			return NULL;
		}
		void *ptr = ((char *)header) + header->headerSize + header->bufferSize * (mCurrentBuffer + 1);
		mBuffer.unlock();
		return ptr;
	}
	return NULL;
}

// Set last finished buffer to current write buffer, then unset current write buffer index.
void SharedMemoryServer::release()
{
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	if (header && mCurrentBuffer >= 0)
	{
		mBuffer.lock();
		header->lastDone = mCurrentBuffer;
		mBuffer.unlock();
		mCurrentBuffer = -1;
	}
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

const void *SharedMemoryClient::buffer()
{
	struct shm_header *header = (struct shm_header *)mBuffer.data();
	if (header)
	{
		if (header->lastDone == -1)
		{
			return NULL; // Nothing 
		}
		mBuffer.lock();
		if (mCurrentBuffer >= 0 && header->buffer[mCurrentBuffer] > 0)
		{
			header->buffer[mCurrentBuffer]--; // Release previous read lock
		}
		header->buffer[header->lastDone]++; // Lock new page against writing
		mCurrentBuffer = header->lastDone;
		const void *ptr = ((const char *)header) + header->headerSize + header->bufferSize * (header->lastDone + 1);
		mBuffer.unlock();
		return ptr;
	}
	return NULL;	
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
