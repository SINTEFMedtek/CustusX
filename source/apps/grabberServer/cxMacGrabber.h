#ifdef __APPLE__
#ifndef CXMACGRABBER_H_
#define CXMACGRABBER_H_

#include "cxGrabber.h"

class QMacCocoaViewContainer;
//class NSError;
//class NSString;

namespace cx
{

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
 * \\author Janne Beate Bakeng, SINTEF
 */
class MacGrabber : public Grabber
{
  //Q_OBJECT

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

//  void reportError(NSError* error); ///< Helper for converting NSError into own messaging system
//  void reportString(NSString* string); ///< Helper for converting NSString into own messaging system
//  void reportError(QString error); ///< Helper for converting NSError into own messaging system
//  void reportString(QString string); ///< Helper for converting NSString into own messaging system

  bool mSuperVideo; ///< Whether or not we are grabbing using super video or not

  /**Helper class for combining objective-c with c++/Qt
  *instead of using void* and reinterpret_cast
  */
  class ObjectiveC;
  ObjectiveC* mObjectiveC;
};
typedef boost::shared_ptr<MacGrabber> MacGrabberPtr;

}//namespace cx

//Declaration of a frame metatype needed to be able to send frame as a signal.
//typedef cx::Frame Frame;
//Q_DECLARE_METATYPE(Frame)

#endif /* CXMACGRABBER_H_ */
#endif /*APPLE*/
