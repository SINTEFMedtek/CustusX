#ifndef CXGUIEXTENDERSERVICETRACKERCUSTOMIZER_H_
#define CXGUIEXTENDERSERVICETRACKERCUSTOMIZER_H_

#include "ctkServiceTracker.h"
#include "ctkServiceTrackerCustomizer.h"
#include "cxGUIExtenderService.h"

namespace cx
{

typedef ctkServiceTracker<GUIExtenderService*> GUIExtenderServiceTracker;
typedef boost::shared_ptr<GUIExtenderServiceTracker> GUIExtenderServiceTrackerPtr;

/**Notify changes in GUIExtenderService lifecycle.
 *
 * Used inside a GUIExtenderServiceTracker.
 *
 * \ingroup cx_gui
 * \date 2014-05-02
 * \author Christian Askeland
 */
class GUIExtenderServiceTrackerCustomizer : public QObject, public ctkServiceTrackerCustomizer<GUIExtenderService*>
{
	Q_OBJECT
public:
	virtual GUIExtenderService* addingService(const ctkServiceReference &reference);
	virtual void modifiedService(const ctkServiceReference &reference, GUIExtenderService* service);
	virtual void removedService(const ctkServiceReference &reference, GUIExtenderService* service);
	virtual ~GUIExtenderServiceTrackerCustomizer();

signals:
	void serviceAdded(GUIExtenderService* reference);
	void serviceRemoved(GUIExtenderService* reference);
};
typedef boost::shared_ptr<GUIExtenderServiceTrackerCustomizer> GUIExtenderServiceTrackerCustomizerPtr;

} /* namespace cx */
#endif /* CXGUIEXTENDERSERVICETRACKERCUSTOMIZER_H_ */

