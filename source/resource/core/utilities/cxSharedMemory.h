/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXSHAREDMEMORY_H_
#define CXSHAREDMEMORY_H_

#include "cxResourceExport.h"

#include <QSharedMemory>
#include <QDateTime>

namespace cx
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
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT SharedMemoryServer
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
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT SharedMemoryClient
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
