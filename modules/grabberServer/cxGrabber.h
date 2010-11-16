#ifndef CXEPIPHANGRABBER_H_
#define CXEPIPHANGRABBER_H_

#include <QObject>
#include <boost/shared_ptr.hpp>

namespace cx
{

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
  Grabber(){};
  virtual ~Grabber(){};
  
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual QWidget* getPreviewWidget() = 0;

signals:
  void frame();
  
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
 * -S-VHS grabber
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
  virtual QWidget* getPreviewWidget();
  
private:
  bool findConnectedDevice();
  bool openDevice();

  //Helper class for combining objective-c with c++/Qt
  //instead of using void* and reinterpret_cast
  class ObjectiveC;
  ObjectiveC* mObjectiveC;
};

typedef boost::shared_ptr<class Grabber> GrabberPtr;

}
#endif /* CXEPIPHANGRABBER_H_ */
