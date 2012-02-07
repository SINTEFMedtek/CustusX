/*
 * cxVector3DComponentDataAdapter.h
 *
 *  \date Jul 25, 2011
 *      \author christiana
 */

#ifndef CXVECTOR3DCOMPONENTDATAADAPTER_H_
#define CXVECTOR3DCOMPONENTDATAADAPTER_H_

#include <sscDoubleDataAdapter.h>
#include "sscVector3DDataAdapter.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/**A data adapter that links to one component of a Vector3DDataAdapter.
 * Useful for displaying widgets for vector components.
 *
 */
class Vector3DComponentDataAdapter : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  Vector3DComponentDataAdapter(ssc::Vector3DDataAdapterPtr base, int index, QString name, QString help);
  virtual ~Vector3DComponentDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(double value);
  virtual double getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual ssc::DoubleRange getValueRange() const;
  virtual double convertInternal2Display(double internal);
  virtual double convertDisplay2Internal(double display);
  virtual int getValueDecimals() const;

  ssc::Vector3DDataAdapterPtr mBase;
  int mIndex;
  QString mName;
  QString mHelp;
};
typedef boost::shared_ptr<Vector3DComponentDataAdapter> Vector3DComponentDataAdapterPtr;


/**
 * @}
 */
}

#endif /* CXVECTOR3DCOMPONENTDATAADAPTER_H_ */
