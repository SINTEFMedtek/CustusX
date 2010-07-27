/*
 * cxLayoutData.cpp
 *
 *  Created on: Jul 27, 2010
 *      Author: christiana
 */
#include "cxLayoutData.h"
#include <iostream>

namespace cx
{


LayoutRegion merge(LayoutRegion a, LayoutRegion b)
{
  int r1 = std::min(a.pos.row, b.pos.row);
  int c1 = std::min(a.pos.col, b.pos.col);
  int r2 = std::max(a.pos.row+a.span.row-1, b.pos.row+b.span.row-1);
  int c2 = std::max(a.pos.col+a.span.col-1, b.pos.col+b.span.col-1);
  return LayoutRegion(r1, c1, r2-r1+1, c2-c1+1);
}


LayoutData::LayoutData()
{
  mSize = LayoutPosition(0,0);
  this->resize(1,3);
}

/** Merge all views inside input region.
 *  Previously merged views partially inside the new region will be split.
 *
 *  Prerequisite: input is inside old boundaries.
 */
void LayoutData::merge(LayoutRegion region)
{
  std::cout << "merge views " << std::endl;
  this->split(region); // split all existing merges in order to keep consistency

  // create new merged view based on the ul corner view.
  ViewData current = this->get(region.pos);
  current.mRegion = region;

  // erase all views within region (all should now be 1x1)
  for (int r=region.pos.row; r<region.pos.row+region.span.row; ++r)
  {
    for (int c=region.pos.col; c<region.pos.col+region.span.col; ++c)
    {
      iterator iter = this->find(LayoutPosition(r,c));
      if (iter!=this->end())
        mView.erase(iter);
    }
  }

  // add merged view.
  mView.push_back(current);
}

/**Split all views wholly or partially within input region
 * into 1x1 views.
 */
void LayoutData::split(LayoutRegion region)
{
  // reset all primitives in region
  for (int r=region.pos.row; r<region.pos.row+region.span.row; ++r)
  {
    for (int c=region.pos.col; c<region.pos.col+region.span.col; ++c)
    {
      iterator iter = this->find(LayoutPosition(r,c));
      this->split(iter);
    }
  }
}

/**Split the region given by iter into 1x1 views.
 *
 */
void LayoutData::split(iterator iter)
{
  if (iter==this->end())
    return;
  if (iter->mRegion.span.row==1 && iter->mRegion.span.col==1)
    return; // nothing to split

  ViewData newView = *iter;
  LayoutRegion region = iter->mRegion;

  // erase old region
  mView.erase(iter);

  // insert new 1x1 views in the erased region.
  for (int r=region.pos.row; r<region.pos.row+region.span.row; ++r)
  {
    for (int c=region.pos.col; c<region.pos.col+region.span.col; ++c)
    {
      newView.mRegion = LayoutRegion(r,c,1,1);
      mView.push_back(newView);
    }
  }
}

/**Resize layout to be of dimension rows x cols.
 *
 */
void LayoutData::resize(int rows, int cols)
{
  mSize = LayoutPosition(rows,cols);

  // erase all views outside region
  // TODO: also consider nontrivial regions.
  for (iterator iter=this->begin(); iter!=this->end(); )
   {
     if (iter->mRegion.pos.row >= rows || iter->mRegion.pos.col >= cols)
     {
       iter = mView.erase(iter);
     }
     else
     {
       ++iter;
     }
   }

  // add new views (brute force: optimize if needed)
  for (int r = 0; r < rows; ++r)
  {
    for (int c = 0; c < cols; ++c)
    {
      if (this->find(LayoutPosition(r,c)) == this->end())
      {
        mView.push_back(ViewData(r,c,1,1));
      }
    }
  }
}

//const LayoutData::ViewData& LayoutData::get(LayoutPosition pos) const
//{
//  return *(this->find(pos));
//}

LayoutData::ViewData& LayoutData::get(LayoutPosition pos)
{
  return *(this->find(pos));
}

LayoutData::iterator LayoutData::find(LayoutPosition pos)
{
  for (iterator iter=this->begin(); iter!=this->end(); ++iter)
  {
    if (iter->mRegion.contains(pos))
      return iter;
  }

  return this->end();
}



} // namespace cx
