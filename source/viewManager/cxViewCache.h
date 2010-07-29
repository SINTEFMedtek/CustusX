/*
 * cxViewCache.h
 *
 *  Created on: Jul 29, 2010
 *      Author: christiana
 */

#ifndef CXVIEWCACHE_H_
#define CXVIEWCACHE_H_

#include <QWidget>
#include <QLayout>
#include <vector>
#include "sscTypeConversions.h"

/**Cache for reuse of Views.
 * Use the retrieve*() method to get views that can be used
 * in layouts. You will get unique views for each call.
 * When rebuilding the gui, remove all views from their layouts,
 * and call clearUsedViews(). The cache will assume all views now
 * are free and ready for reuse.
 *
 */
template <class VIEW_TYPE>
class ViewCache
{
public:
  ViewCache(QWidget* widget, QString typeText) : mCentralWidget(widget), mNameGenerator(0), mTypeText(typeText)
  {
  }
  /**Retrieve a view that is unique since the last call to clearUsedViews()
   */
  VIEW_TYPE* retrieveView()
  {
    if (mCached.empty())
    {
      std::string uid = string_cast(mTypeText) + "-" + string_cast(mNameGenerator++);
      VIEW_TYPE* view = new VIEW_TYPE(uid, uid, mCentralWidget);
      view->hide();
      //Turn off rendering in vtkRenderWindowInteractor
      view->getRenderWindow()->GetInteractor()->EnableRenderOff();
      mCached.push_back(view);
    }

    VIEW_TYPE* retval = mCached.back();
    mCached.pop_back();
    mUsed.push_back(retval);
    return retval;
  }
  /**Reset the cache for new use.
   * Remove all used views from the central widget and hide them.
   */
  void clearUsedViews()
  {
    for (unsigned i=0; i<mUsed.size(); ++i)
    {
      mUsed[i]->hide();
      mCentralWidget->layout()->removeWidget(mUsed[i]);
    }

    std::copy(mUsed.begin(), mUsed.end(), back_inserter(mCached));
    mUsed.clear();
  }
private:
  QWidget* mCentralWidget;  ///< should not be used after stealCentralWidget has been called, because then MainWindow owns it!!!
  int mNameGenerator;
  QString mTypeText;
  std::vector<VIEW_TYPE*> mCached;
  std::vector<VIEW_TYPE*> mUsed;
};


#endif /* CXVIEWCACHE_H_ */
