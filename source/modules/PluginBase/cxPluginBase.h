/*
 * cxPluginBase.h
 *
 *  \date Jun 14, 2011
 *      \author christiana
 */

#ifndef CXPLUGINBASE_H_
#define CXPLUGINBASE_H_

#include <QObject>
#include "boost/shared_ptr.hpp"
class QWidget;
class QToolBar;
#include <vector>

namespace cx
{
typedef boost::shared_ptr<class PluginBase> PluginBasePtr;

/** Interface for plugins.
 *
 */
class PluginBase : public QObject
{
public:
	struct PluginWidget
	{
		PluginWidget() {}
		PluginWidget(QWidget* widget, QString category) : mWidget(widget), mCategory(category) {}
		QWidget* mWidget;
		QString mCategory;
	};
//	PluginBase();
	virtual ~PluginBase() {}

	virtual std::vector<PluginWidget> createWidgets() const = 0;
	virtual std::vector<QToolBar*> createToolBars() const { return std::vector<QToolBar*>(); }
	//	virtual void addXml(QDomNode& dataNode);
//	virtual void parseXml(QDomNode& dataNode);
};

}

#endif /* CXPLUGINBASE_H_ */
