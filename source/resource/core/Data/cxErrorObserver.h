#ifndef CXERROROBSERVER_H
#define CXERROROBSERVER_H

#include "cxResourceExport.h"
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <vtkSmartPointer.h>
#include <vtkAlgorithm.h>
#include <vtkCommand.h>

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


/** Wrapper for vtkAlgorithm::Update(),
  * prints error message upon error,
  * also wraps the call inside a global mutex (see below for why).
  *
  * \ingroup sscData
  * \date jan 1, 2010
  * \date april 17, 2013
  * \author christiana
  */
class cxResource_EXPORT ErrorObserver: public vtkCommand
{
public:
	ErrorObserver() {}
	static ErrorObserver* New();
	virtual void Execute(vtkObject* caller, unsigned long, void* text);

	static bool checkedRead(vtkSmartPointer<vtkAlgorithm> reader, QString filename);

	QString mMessage;
};

}
#endif // CXERROROBSERVER_H