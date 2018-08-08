#ifndef CXERROROBSERVER_H
#define CXERROROBSERVER_H

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <vtkCommand.h>
#include <vtkSmartPointer.h>
#include <vtkAlgorithm.h>
#include <QString>
#include "cxLogger.h"
#include <QMutex>

namespace cx {

/** Locks a static mutex in the constructor and unlocks it in the desctructor,
  * similar to a QMutexLocker.
  *
  * Use this as a global access restriction for thread-unsafe VTK objects.
  *
  * Testing has shown that the following methods need to be mutexed:
  *   - vtkMetaImageReader::Update()
  *   - vtkMetaImageWrite::Write()
  * There are probably some global stuff inside vtkmetaio.
  *
  * Note: Googling indicates that VTK in general is threadUNsafe.
  */
class cxResource_EXPORT StaticMutexVtkLocker
{
public:
	StaticMutexVtkLocker();
	~StaticMutexVtkLocker();
private:
	static boost::shared_ptr<QMutex> mMutex;
};


//---------------------------------------------------------
/** Wrapper for vtkAlgorithm::Update(),
  * prints error message upon error,
  * also wraps the call inside a global mutex (see below for why).
  *
  */
class cxResource_EXPORT ErrorObserver: public vtkCommand
{
public:
	ErrorObserver()
	{
	}
	static ErrorObserver* New()
	{
		return new ErrorObserver;
	}
	virtual void Execute(vtkObject* caller, unsigned long, void* text)
	{
		mMessage = QString(reinterpret_cast<char*> (text));
	}
	QString mMessage;

	static bool checkedRead(vtkSmartPointer<vtkAlgorithm> reader, QString filename)
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
};

}

#endif // CXERROROBSERVER_H
