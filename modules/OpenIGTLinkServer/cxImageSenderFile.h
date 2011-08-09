/*
 * cxImageSenderFile.h
 *
 *  Created on: Jun 21, 2011
 *      Author: christiana
 */

#ifndef CXIMAGESENDERFILE_H_
#define CXIMAGESENDERFILE_H_

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

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 */
class MHDImageSender : public QObject
{
  Q_OBJECT

public:
  MHDImageSender(QTcpSocket* socket, StringMap arguments, QObject* parent = NULL);
  virtual ~MHDImageSender() {}

  static QString getType();
  static QStringList getArgumentDescription();

protected:
private:
  QTcpSocket* mSocket;
  QTimer* mTimer;
  int mCounter;
  vtkImageDataPtr mImageData;
  StringMap mArguments;
  std::vector<unsigned char> mTestData;
  vtkImageImportPtr mImageImport;
  void setTestImage();
private slots:
  void tick();

};


}

#endif /* CXIMAGESENDERFILE_H_ */
