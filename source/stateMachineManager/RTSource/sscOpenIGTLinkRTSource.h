/*
 * sscOpenIGTLinkRTSource.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#ifndef SSCOPENIGTLINKRTSOURCE_H_
#define SSCOPENIGTLINKRTSOURCE_H_

#include "sscRealTimeStreamSource.h"
#include <boost/array.hpp>
#include "igtlImageMessage.h"
#include <vector>
class QTimer;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;

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
  virtual QString getUid() { return "us_openigtlink_source"; }
  virtual QString getName();
  virtual vtkImageDataPtr getVtkImageData();
  virtual double getTimestamp();
  virtual bool isConnected() const;

  virtual QString getInfoString() const;
  virtual QString getStatusString() const;

  virtual void start();
//  virtual void pause();
  virtual void stop();

  virtual bool validData() const;
  virtual bool isStreaming() const;

  // non-inherited methods
  void connectServer(QString address, int port);
  void disconnectServer();
  void setTimestampCalibration(double delta);

//signals:
//  void serverStatusChanged();

public:
  void updateImage(igtl::ImageMessage::Pointer message); // called by receiving thread when new data arrives.

private slots:
  void clientFinishedSlot();
  void imageReceivedSlot();
  void timeout();
  void fpsSlot(double fps);
  void connectedSlot(bool on);

private:
  void setEmptyImage();
  std::vector<unsigned char> mTestData;
  void setTestImage();
  vtkImageDataPtr createFilterARGB2RGBA(vtkImageDataPtr input);
  void updateImageImportFromIGTMessage(igtl::ImageMessage::Pointer message);

  boost::array<unsigned char, 100> mZero;
  vtkImageImportPtr mImageImport;
  vtkImageDataPtr mFilter_ARGB_RGBA;
//  QDateTime mTimestamp;
  vtkImageAlgorithmPtr mRedirecter;
  igtl::ImageMessage::Pointer mImageMessage;
  IGTLinkClientPtr mClient;
  bool mConnected;
  QString mDeviceName;
  bool mTimeout;
  QTimer* mTimeoutTimer;
  double mFPS;
  double mLastTimestamp;
  double mTimestampCalibration;
};
typedef boost::shared_ptr<OpenIGTLinkRTSource> OpenIGTLinkRTSourcePtr;

} // namespace ssc

#endif /* SSCOPENIGTLINKRTSOURCE_H_ */
