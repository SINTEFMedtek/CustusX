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

  void updateImage(igtl::ImageMessage::Pointer message); // called by receiving thread when new data arrives.

signals:
  void newData();

public:
//  void connect(int shmtKey);
//  void reconnect();
//  void disconnect();
//  void update();
  int width() const { return mImageWidth; }
  int height() const { return mImageHeight; }

private:
  void initializeBuffer(int newWidth, int newHeight);
  void padBox(int* x, int* y) const;

  //int m_shmtKey;
  //int mUSSession;
  uint32_t *mUSTextBuf;
  int mImageWidth;
  int mImageHeight;
  vtkImageDataPtr mImageData;
  vtkImageImportPtr mImageImport;
  QDateTime mTimestamp;
  igtl::ImageMessage::Pointer mImageMessage;
};
typedef boost::shared_ptr<OpenIGTLinkRTSource> OpenIGTLinkRTSourcePtr;

} // namespace ssc

#endif /* SSCOPENIGTLINKRTSOURCE_H_ */
