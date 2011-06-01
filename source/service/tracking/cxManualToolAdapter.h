/*
 * cxManualToolAdapter.h
 *
 *  Created on: Feb 14, 2011
 *      Author: christiana
 */

#ifndef CXMANUALTOOLADAPTER_H_
#define CXMANUALTOOLADAPTER_H_

#include "sscManualTool.h"

namespace cx
{

/**A ManualToolAdapter inherits from manual tool, but also
 * contains a cx::Tool that is requests shape and probe info from.
 *
 * Used for debug - when testing tools without a tracking system.
 *
 */
class ManualToolAdapter : public ssc::ManualTool
{
  Q_OBJECT
public:
  explicit ManualToolAdapter(QString uid);
  explicit ManualToolAdapter(ssc::ToolPtr base);
  virtual ~ManualToolAdapter();
  virtual Type getType() const;
  virtual QString getGraphicsFileName() const;
  virtual vtkPolyDataPtr getGraphicsPolyData() const;
//  virtual QString getUid() const;
//  virtual QString getName() const;
  virtual bool isCalibrated() const;
  virtual ssc::ProbeData getProbeSector() const;
  virtual ssc::ProbePtr getProbe() const;

  virtual ssc::Transform3D getCalibration_sMt() const;
  virtual std::map<int, ssc::Vector3D> getReferencePoints() const;

  void setBase(ssc::ToolPtr base);

private:
  ssc::ToolPtr mBase;
};

typedef boost::shared_ptr<ManualToolAdapter> ManualToolAdapterPtr;


}

#endif /* CXMANUALTOOLADAPTER_H_ */
