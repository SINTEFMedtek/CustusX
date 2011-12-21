/*
 * cxOpenIGTLinkRTSource.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#ifndef CXOPENIGTLINKRTSOURCE_H_
#define CXOPENIGTLINKRTSOURCE_H_

#include "sscVideoSource.h"
#include <boost/array.hpp>
#include "igtlImageMessage.h"
#include <vector>
#include "../../../modules/grabberCommon/cxIGTLinkSonixStatusMessage.h"
class QTimer;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;

namespace cx
{

typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

/**Synchronize data with source,
 * provide data as a vtkImageData.
 */
class OpenIGTLinkRTSource : public ssc::VideoSource
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
  virtual void release() {};

  // non-inherited methods
  void connectServer(QString address, int port);
  void disconnectServer();
  void setTimestampCalibration(double delta);
  double getTimestampCalibration() { return mTimestampCalibration; }
  void setSoundSpeedCompensation(double gamma); ///< gamma is the correction factor for the distance along the sound direction

signals:
  void fps(int fps);

public:
  void updateImage(igtl::ImageMessage::Pointer message); // called by receiving thread when new data arrives.
  void updateSonixStatus(IGTLinkSonixStatusMessage::Pointer message);

private slots:
  void clientFinishedSlot();
  void imageReceivedSlot();
  void sonixStatusReceivedSlot();
  void timeout();
  void fpsSlot(double fps);
  void connectedSlot(bool on);

private:
  void setEmptyImage();
  std::vector<unsigned char> mTestData;
  void setTestImage();
  vtkImageDataPtr createFilterARGB2RGBA(vtkImageDataPtr input);
  void updateImageImportFromIGTMessage(igtl::ImageMessage::Pointer message);

  double mDebug_orgTime;

  boost::array<unsigned char, 100> mZero;
  vtkImageImportPtr mImageImport;
  vtkImageDataPtr mFilter_ARGB_RGBA;
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
  double mLinearSoundSpeedCompesation;

  int mSize[3]; // image dimension
  float mOrigin[3];
};
typedef boost::shared_ptr<OpenIGTLinkRTSource> OpenIGTLinkRTSourcePtr;

} // namespace cx

#endif /* CXOPENIGTLINKRTSOURCE_H_ */
