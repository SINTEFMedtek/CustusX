/*
 * cxLayoutData.h
 *
 *  Created on: Jul 27, 2010
 *      Author: christiana
 */

#ifndef CXLAYOUTDATA_H_
#define CXLAYOUTDATA_H_

#include <vector>
class QDomNode;
#include <QString>
#include "sscDefinitions.h"

namespace cx
{

struct LayoutPosition
{
  LayoutPosition() {}
  LayoutPosition(int r, int c) : row(r), col(c) {}
  int row; ///< row
  int col; ///< column
};

struct LayoutRegion
{
  LayoutRegion() {}
  LayoutRegion(int r, int c, int rSpan=1, int cSpan=1) : pos(r,c), span(rSpan, cSpan) {}
  LayoutPosition pos; ///< start position of region
  LayoutPosition span; ///< size of region

  bool contains(LayoutPosition p) const
  {
    return pos.row<=p.row && p.row<pos.row+span.row
        && pos.col<=p.col && p.col<pos.col+span.col;
  }
};

LayoutRegion merge(LayoutRegion a, LayoutRegion b);


/** Represents one specific layout of the views, and what kind of information they contain,
 *  i.e 3D/2D, slice plane definition, image group.
 */
class LayoutData
{
public:
  /** Describes the layout and content of one view.
   */
  struct ViewData
  {
    ViewData() : mGroup(-1), mPlane(ssc::ptCOUNT), mRegion(-1,-1,1,1) {}
    ViewData(int row, int col, int rowSpan=1, int colSpan=1) : mGroup(-1), mPlane(ssc::ptCOUNT), mRegion(row,col,rowSpan,colSpan) {}
    int mGroup; ///< what group to connect to. -1 means not set.
    ssc::PLANE_TYPE mPlane; ///< ptNOPLANE means 3D
    LayoutRegion mRegion;

    void addXml(QDomNode node) const; ///< save state to xml
    void parseXml(QDomNode node);///< load state from xml
  };

  typedef std::vector<ViewData> ViewDataContainer;
  typedef ViewDataContainer::iterator iterator;
  typedef ViewDataContainer::const_iterator const_iterator;

public:
  LayoutData();
  QString getUid() const { return mUid; }
  QString getName() const { return mName; }
  void setName(const QString& name) { mName = name; }
  void resetUid(const QString& uid);
  void setView(int group, ssc::PLANE_TYPE type, LayoutRegion region);
//  const ViewData& get(LayoutPosition pos) const;
  ViewData& get(LayoutPosition pos);
  //void set(ViewData view);
  iterator begin() { return mView.begin(); }
  iterator end()  { return mView.end(); }
  const_iterator begin() const { return mView.begin(); }
  const_iterator end() const { return mView.end(); }
  iterator find(LayoutPosition pos);

  void split(iterator iter);
  void split(LayoutRegion region);
  bool merge(LayoutRegion region);
  void resize(int rows, int cols);
  LayoutPosition size() const { return mSize; }

  void addXml(QDomNode node) const; ///< save state to xml
  void parseXml(QDomNode node);///< load state from xml

private:
  QString mUid;
  QString mName;
  LayoutPosition mSize;
  std::vector<ViewData> mView;
};


} // namespace cx

#endif /* CXLAYOUTDATA_H_ */
