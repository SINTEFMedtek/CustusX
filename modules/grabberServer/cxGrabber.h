#ifndef CXEPIPHANGRABBER_H_
#define CXEPIPHANGRABBER_H_

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
  float mTimestamp;
  int mWidth;
  int mHeight;
  int mPixelFormat;
  char* mFirstPixel;
};

/**
 * \class Grabber
 *
 * \brief
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
  
  virtual void start(){};
  virtual void stop(){};
  virtual bool isGrabbing(){return false;};
  virtual void displayPreview(QWidget* parent){};

signals:
  void frame(Frame& frame);
  void started();
  void stopped();
  
protected:
};


/**
 * \class MacGrabber
 *
 * \brief
 *
 * Supported grabbers are:
 * -new VGA grabber (Epiphan)
 * -old VGA grabber (Epiphan)
 * -S-VIDEO grabber
 * -buildt in apple i-sight camera
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

  void sendFrame(Frame& frame);
  
  void printAvailablePixelFormats();

private:
  bool findConnectedDevice();
  bool openDevice();
  bool closeDevice();
  
  void startSession();
  void stopSession();
  
  void setupGrabbing();

  void reportError(NSError* error);
  void reportString(NSString* string);

  //Helper class for combining objective-c with c++/Qt
  //instead of using void* and reinterpret_cast
  class ObjectiveC;
  ObjectiveC* mObjectiveC;
};

typedef boost::shared_ptr<class Grabber> GrabberPtr;
typedef boost::shared_ptr<class MacGrabber> MacGrabberPtr;

}//namespace cx

typedef cx::Frame Frame;
Q_DECLARE_METATYPE(Frame)

#endif /* CXEPIPHANGRABBER_H_ */
