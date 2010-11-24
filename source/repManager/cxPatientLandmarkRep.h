#ifndef CXPATIENTLANDMARKREP_H_
#define CXPATIENTLANDMARKREP_H_

#include "cxLandmarkRep.h"

namespace cx
{
typedef boost::shared_ptr<class PatientLandmarkRep> PatientLandmarkRepPtr;
/**
 * \class PatientLandmarkRep
 *
 * \brief
 *
 * \date 23. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

class PatientLandmarkRep : public LandmarkRep
{
public:
  static PatientLandmarkRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~PatientLandmarkRep();

  virtual QString getType() const;

public slots:
  virtual void landmarkAddedSlot(QString );
  virtual void transformChangedSlot();

protected:
  PatientLandmarkRep(const QString& uid, const QString& name=""); ///< sets default text scaling to 20
  virtual void clearAll();
  virtual void addAll();
  virtual void setPosition(ssc::Vector3D coord, QString uid);
};

}//namespace cx
#endif /* CXPATIENTLANDMARKREP_H_ */
