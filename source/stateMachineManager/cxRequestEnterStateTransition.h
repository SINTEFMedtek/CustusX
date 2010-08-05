#ifndef CXREQUESTENTERSTATETRANSITION_H_
#define CXREQUESTENTERSTATETRANSITION_H_

#include <QEvent>
#include <QAbstractTransition>

namespace cx
{

struct RequestEnterStateEvent : public QEvent
{
  RequestEnterStateEvent(const QString &stateUid)
    : QEvent(QEvent::Type(QEvent::User+1)),
      mStateUid(stateUid) {}

    QString mStateUid;
};

/**
 * \class cxRequestEnterStateTransition.h
 *
 * \brief
 *
 * \date 5. aug. 2010
 * \author: jbake
 */
class RequestEnterStateTransition: public QAbstractTransition
{
public:
  RequestEnterStateTransition(const QString &stateUid) :
    mStateUid(stateUid)
  {
  }

protected:
  virtual bool eventTest(QEvent *e)
  {
    if (e->type() != QEvent::Type(QEvent::User + 1)) // StringEvent
      return false;
    RequestEnterStateEvent *se = static_cast<RequestEnterStateEvent*> (e);
    return (mStateUid == se->mStateUid);
  }

  virtual void onTransition(QEvent *) {}

private:
  QString mStateUid;
};

}//namespace cx
#endif /* CXREQUESTENTERSTATETRANSITION_H_ */
