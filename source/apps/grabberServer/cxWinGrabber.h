#ifdef WIN32
#ifndef CXWINGRABBER_H_
#define CXWINGRABBER_H_

#include "cxGrabber.h"
#include "SonixHelper.h"

class QMacCocoaViewContainer;
class NSError;
class NSString;
class vtkSonixVideoSource;

namespace cx
{

/**
 * \class WinGrabber
 *
 * \brief A grabber that only works on Windows because it uses Ulterius.
 *
 * \date 06. jun. 2011
 * \\author Janne Beate Bakeng, SINTEF
 */
class WinGrabber : public Grabber
{
  Q_OBJECT

public:
  WinGrabber();
  virtual ~WinGrabber();
  
  virtual void start();
  virtual void stop();
  virtual bool isGrabbing();
  virtual void displayPreview(QWidget* parent);

  void sendFrame(Frame& frame); ///< Used by delegate that is listening to arriving frames, to send the new frame via the grabber.
  void setIpAdress(QString ipAdress);
  //void printAvailablePixelFormats(); ///< For debugging purposes, displays a list of available pixelformats that effectivly can be delivered.

  SonixHelper* getSonixHelper();

private:
  bool findConnectedDevice(); ///< Searches for connected devices and connects when it finds a supported device. (The last it finds)
  bool openDevice(); ///< Tries to open the selected device for grabbing.
  bool closeDevice(); ///< Tries to close the selected device for grabbing.
  
  void startSession(); ///< Sets up the grabbing session and requests that grabbing will be started
  void stopSession(); ///< Requests that the grabbing stops
  
  void setupGrabbing(); ///< Configures the grabbing session with specified frame formats.

  //void reportError(NSError* error); ///< Helper for converting NSError into own messaging system
  //void reportString(NSString* string); ///< Helper for converting NSString into own messaging system

  ///**Helper class for combining objective-c with c++/Qt
  //*instead of using void* and reinterpret_cast
  //*/
  //class ObjectiveC;
  //ObjectiveC* mObjectiveC;
  //bool mSuperVideo;

  vtkSonixVideoSource* mSonixGrabber; ///<
  QString              mIpAdressToConnectTo; ///< 
  int                  mBufferSize; ///< 
  int                  mImagingMode;  ///< 0=B-MODE, 12=RF-MODE
  int                  mAcquisitionDataType;  ///< 0x00000002=udtBPre, 0x00000004=udtBPost

  SonixHelper *mSonixHelper; ///< Support Qt functionality to vtkSonixVideoSource

};
typedef boost::shared_ptr<class WinGrabber> WinGrabberPtr;

}//namespace cx

//Declaration of a frame metatype needed to be able to send frame as a signal.
//typedef cx::Frame Frame;
//Q_DECLARE_METATYPE(Frame)

#endif /* CXWINGRABBER_H_ */
#endif /*WIN32*/
