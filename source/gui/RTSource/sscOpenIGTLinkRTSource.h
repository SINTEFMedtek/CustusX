/*
 * sscOpenIGTLinkRTSource.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef SSCOPENIGTLINKRTSOURCE_H_
#define SSCOPENIGTLINKRTSOURCE_H_

#include "sscRealTimeSource.h"

#include "igtlImageMessage.h"
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;


namespace ssc
{

typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;


/**Synchronize data with source,
 * provide data as a vtkImageData.
 */
class OpenIGTLinkRTSource : public RealTimeStreamSource
{
  Q_OBJECT
public:
  OpenIGTLinkRTSource();
  virtual ~OpenIGTLinkRTSource();
  virtual vtkImageDataPtr getVtkImageData();
  virtual QDateTime getTimestamp();
  virtual bool connected() const;

  // non-inherited methods
  void connectServer(QString address, int port);
  void disconnectServer();


signals:
  void newData();
  void serverStatusChanged();

public:
  void updateImage(igtl::ImageMessage::Pointer message); // called by receiving thread when new data arrives.

private slots:
  void clientFinishedSlot();
  void imageReceivedSlot();

private:
//  void padBox(int* x, int* y) const;
  void setEmptyImage();

  unsigned char mZero;
  vtkImageImportPtr mImageImport;
  QDateTime mTimestamp;
  igtl::ImageMessage::Pointer mImageMessage;
  IGTLinkClientPtr mClient;

};
typedef boost::shared_ptr<OpenIGTLinkRTSource> OpenIGTLinkRTSourcePtr;

} // namespace ssc

#endif /* SSCOPENIGTLINKRTSOURCE_H_ */
