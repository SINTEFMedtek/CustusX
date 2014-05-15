#ifndef CXGUIEXTENDERSERVICE_H_
#define CXGUIEXTENDERSERVICE_H_

#include <QObject>
#include "boost/shared_ptr.hpp"
class QWidget;
class QToolBar;
#include <vector>

#define GUIExtenderService_iid "cx::GUIExtenderService"

namespace cx
{
typedef boost::shared_ptr<class GUIExtenderService> GUIExtenderServicePtr;

/** Interface for service that extends the user interface with more widgets.
 *
 */
class GUIExtenderService : public QObject
{
	Q_OBJECT
public:
	struct CategorizedWidget
	{
		CategorizedWidget() {}
		CategorizedWidget(QWidget* widget, QString category) : mWidget(widget), mCategory(category) {}
		QWidget* mWidget;
		QString mCategory;
	};
	virtual ~GUIExtenderService() {}

	virtual std::vector<CategorizedWidget> createWidgets() const = 0;
//	virtual std::vector<QToolBar*> createToolBars() const { return std::vector<QToolBar*>(); }
};

} // namespace cx
Q_DECLARE_INTERFACE(cx::GUIExtenderService, GUIExtenderService_iid)


#endif /* CXGUIEXTENDERSERVICE_H_ */
