#include "cxErrorObserver.h"

#include "cxLogger.h"

namespace cx {


//---------------------------------------------------------
StaticMutexVtkLocker::StaticMutexVtkLocker()
{
/*	if (!mMutex)
		mMutex.reset(new QMutex(QMutex::Recursive));

	mMutex->lock();*/
}
StaticMutexVtkLocker::~StaticMutexVtkLocker()
{
//	mMutex->unlock();
}
boost::shared_ptr<QMutex> StaticMutexVtkLocker::mMutex;

ErrorObserver *ErrorObserver::New()
{
	return new ErrorObserver;
}

void ErrorObserver::Execute(vtkObject *caller, unsigned long, void *text)
{
	mMessage = QString(reinterpret_cast<char*> (text));
}

bool ErrorObserver::checkedRead(vtkSmartPointer<vtkAlgorithm> reader, QString filename)
{
	vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
	reader->AddObserver("ErrorEvent", errorObserver);

	{
		StaticMutexVtkLocker lock;
		reader->Update();
	}
	//		ErrorObserver::threadSafeUpdate(reader);

	if (!errorObserver->mMessage.isEmpty())
	{
		reportError("Load of data [" + filename + "] failed with message:\n"
					+ errorObserver->mMessage);
		return false;
	}
	return true;
}

//---------------------------------------------------------


}
