#ifndef CXGRABBER_H_
#define CXGRABBER_H_

#include <QObject>
#include <QMetaType>
#include <boost/shared_ptr.hpp>

namespace cx
{
class Frame
{
public:
  double mTimestamp; ///< Timestamp in seconds since 1/1/1970 (epoch)
  int mWidth; ///< Width in pixels
  int mHeight; ///< Height in pixels
  int mPixelFormat; ///< Pixel format in OSType (FourCC)
  unsigned char* mFirstPixel; ///< Pointer to first pixel in frame
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

typedef boost::shared_ptr<Grabber> GrabberPtr;
}//namespace cx

//Declaration of a frame metatype needed to be able to send frame as a signal.
typedef cx::Frame Frame;
Q_DECLARE_METATYPE(Frame)

#endif /* CXGRABBER_H_ */
