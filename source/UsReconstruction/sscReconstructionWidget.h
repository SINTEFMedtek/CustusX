/*
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTIONWIDGET_H_
#define SSCRECONSTRUCTIONWIDGET_H_

#include <QWidget>
#include "sscReconstructer.h"


namespace ssc
{
class ReconstructionWidget : public QWidget
{
  Q_OBJECT
public:
  ReconstructionWidget(QWidget* parent);
//private:
  ReconstructerPtr mReconstructer;
  
  QString mInputFile;
};

}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_