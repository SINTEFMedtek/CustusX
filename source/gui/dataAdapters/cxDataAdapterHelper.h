#ifndef CXDATAADAPTERHELPER_H_
#define CXDATAADAPTERHELPER_H_

#include "sscDataAdapter.h"

class QWidget;
class QGridLayout;

namespace cx {

/**\brief Create a widget capable of displaying the input data.
 *
 * If a gridLayout is provided, the widget will insert its components
 * into a row in that layout
 *
 * \ingroup cxGUI
 */
QWidget* createDataWidget(QWidget* parent, DataAdapterPtr data, QGridLayout* gridLayout = 0, int row = 0);


} /* namespace cx */
#endif /* CXDATAADAPTERHELPER_H_ */
