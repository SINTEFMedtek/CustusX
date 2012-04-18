/*
 * cxImageServer.h
 *
 *  \date Oct 30, 2010
 *      \author christiana
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

class QuitCheck : public QThread
{
	Q_OBJECT
public:
	QuitCheck(QObject* parent = NULL);
protected:
	void run();
};

//class ImageSession : public QObject
//{
//  Q_OBJECT
//
//public:
//  ImageSession(int socketDescriptor, ImageSenderPtr imageSender, QObject* parent = NULL);
//  virtual ~ImageSession();
//protected:
//  void run();
//private:
//  ImageSenderPtr mImageSender;
//  QTcpSocket* mSocket;
//  int mSocketDescriptor;
//	StringMap mArguments;
//
//private slots:
//
//};
//
//
//typedef boost::shared_ptr<class ImageSession> ImageSessionPtr;


} // namespace cx

#endif /* CXIMAGESESSION_H_ */
