#ifndef CXGRABBER_H_
#define CXGRABBER_H_

#include <QObject>
#include <QMetaType>
#include <boost/shared_ptr.hpp>

class QMacCocoaViewContainer;
class NSError;
class NSString;

namespace cx
{
class Frame
{
public:
  float mTimestamp; ///< Timestamp in milliseconds
  int mWidth; ///< Width in pixels
  int mHeight; ///< Height in pixels
  int mPixelFormat; ///< Pixel format in OSType (FourCC)
  char* mFirstPixel; ///< Pointer to first pixel in frame
};

/**
 * \class Grabber
 *
 * \brief Generic grabber that presents a interface, but is unable to connect
 * to any underlying hardware.
 *
 * \date 16. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class Grabber : public QObject
{
  Q_OBJECT

public:
  Grabber();
  virtual ~Grabber(){};
  
  virtual void start(){}; ///< Start grabbing.
  virtual void stop(){}; ///< Stop grabbing.
  virtual bool isGrabbing(){return false;}; ///< Whether or not the grabber is grabbing atm.
  virtual void displayPreview(QWidget* parent){}; ///< Places a previewwidget into the parent widget.

signals:
  void frame(Frame& frame); ///< Emitted when new frame arrived
  void started(); ///< Emitted when the grabber started grabbing.
  void stopped(); ///< Emitted when the grabber stopped grabbing.
  
protected:
};

/**
 * \class MacGrabber
 *
 * \brief A grabber that only works on Mac OS because it uses QtKit.
 *
 * Supported grabbers are:
 * -new VGA grabber (Epiphan)
 * -old VGA grabber (Epiphan)
 * -S-VIDEO grabber
 * -buildt in apple i-sight camera
 *
 * \warning Only delivers 32bit ARGB frames at the moment.
 * \warning QtKit only supports 32 bit drivers for capturing hardware atm.
 *
 * \date 16. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class MacGrabber : public Grabber
{
  Q_OBJECT

public:
  MacGrabber();
  virtual ~MacGrabber();
  
  virtual void start();
  virtual void stop();
  virtual bool isGrabbing();
  virtual void displayPreview(QWidget* parent);

  void sendFrame(Frame& frame); ///< Used by delegate that is listening to arriving frames, to send the new frame via the grabber.
  void printAvailablePixelFormats(); ///< For debugging purposes, displays a list of available pixelformats that effectivly can be delivered.

private:
  bool findConnectedDevice(); ///< Searches for connected devices and connects when it finds a supported device. (The last it finds)
  bool openDevice(); ///< Tries to open the selected device for grabbing.
  bool closeDevice(); ///< Tries to close the selected device for grabbing.
  
  void startSession(); ///< Sets up the grabbing session and requests that grabbing will be started
  void stopSession(); ///< Requests that the grabbing stops
  
  void setupGrabbing(); ///< Configures the grabbing session with specified frame formats.

  void reportError(NSError* error); ///< Helper for converting NSError into own messaging system
  void reportString(NSString* string); ///< Helper for converting NSString into own messaging system

  /**Helper class for combining objective-c with c++/Qt
  *instead of using void* and reinterpret_cast
  */
  class ObjectiveC;
  ObjectiveC* mObjectiveC;
};

typedef boost::shared_ptr<class Grabber> GrabberPtr;
typedef boost::shared_ptr<class MacGrabber> MacGrabberPtr;

}//namespace cx

//Declaration of a frame metatype needed to be able to send frame as a signal.
typedef cx::Frame Frame;
Q_DECLARE_METATYPE(Frame)

#endif /* CXGRABBER_H_ */
