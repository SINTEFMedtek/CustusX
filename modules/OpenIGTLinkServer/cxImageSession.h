/*
 * cxImageServer.h
 *
 *  Created on: Oct 30, 2010
 *      Author: christiana
 */

#ifndef CXIMAGESESSION_H_
#define CXIMAGESESSION_H_

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QThread>
class QTimer;
#include "igtlImageMessage.h"
#include "cxImageSenderFactory.h"
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx
{

class ImageSession : public QThread
{
  Q_OBJECT

public:
  ImageSession(int socketDescriptor, QObject* parent = NULL);
  virtual ~ImageSession();
protected:
  void run();
private:
  QTcpSocket* mSocket;
  int mSocketDescriptor;
	StringMap mArguments;

private slots:

};

} // namespace cx

#endif /* CXIMAGESESSION_H_ */
