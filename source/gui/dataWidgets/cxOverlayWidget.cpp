#include "cxOverlayWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxViewWrapper2D.h"

namespace cx {

OverlayWidget::OverlayWidget(QWidget* parent) :
		BaseWidget(parent, "OverlayWidget", "Overlay")
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new TransferFunction2DOpacityWidget(this), 0);
	layout->addWidget(new DataViewSelectionWidget(this), 1);
	this->setEnabled(ViewWrapper2D::overlayIsEnabled());
}

QString OverlayWidget::defaultWhatsThis() const{
	return "<html>"
			"<h3>2D overlay.</h3>"
			"<p>"
			"Allow visualizing several 2D slices  in top of each other "
			"by making them partly transparent (alpha), "
			"or by making parts of the fully transparent (LLR)"
			"</p>"
			"<p>"
			"<b>NB!</b> The functionality in this widget only works on Linux for now"
			"</p>"
			"<p><i></i></p>"
			"</html>";
}

} /* namespace cx */
