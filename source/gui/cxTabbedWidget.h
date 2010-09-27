#ifndef CXTABBEDWIDGET_H_
#define CXTABBEDWIDGET_H_

#include <QWidget>
#include <QString>

class QTabWidget;

namespace cx
{
/**
 * \class TabbedWidget
 *
 * \brief Interface for making widgets with a hierarchy of tabs
 *
 * \date 27. sep. 2010
 * \author: Janne Beate Bakeng
 */

class TabbedWidget : public QWidget
{
public:
  TabbedWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~TabbedWidget();

  void addTab(QWidget* newTab, QString newTabName);

private:
  QTabWidget* mTabWidget;

};
}
#endif /* CXTABBEDWIDGET_H_ */
