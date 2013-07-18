#include "cxColorWidget.h"
#include "cxTransferFunctionPresetWidget.h"
#include "cxTransferFunction2DColorWidget.h"

namespace cx {

ColorWidget::ColorWidget(QWidget* parent) :
		BaseWidget(parent, "ColorWidget", "Color")
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new TransferFunction2DColorWidget(this), 0);
	layout->addStretch(1);
	layout->addWidget(new TransferFunctionPresetWidget(this, false), 0);
}

QString ColorWidget::defaultWhatsThis() const{
	return "<html>"
			"<h3>2D coloring.</h3>"
			"<p>"
			"Allows setting 2D image properties "
			"by setting colors and window with and level"
			"</p>"
			"<p><i></i></p>"
			"</html>";
}

} /* namespace cx */
