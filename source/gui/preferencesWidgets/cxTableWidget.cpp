#include "cxTableWidget.h"
#include <QLayout>
#include <QLabel>
#include "cxPatientModelService.h"
#include "cxTransform3DWidget.h"
#include "cxVector3DProperty.h"
#include "cxHelperWidgets.h"
#include "cxVector3DWidget.h"
#include <QPushButton>
#include "cxSpaceProvider.h"

namespace cx
{

TableWidget::TableWidget(CoreServicesPtr services, QWidget* parent) :
	QWidget(parent),
	mServices(services)
{
	mVerticalLayout = new QVBoxLayout(this);

	mVerticalLayout->setMargin(0);

	mVerticalLayout->addWidget(new QLabel("Define table up:"));

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mVerticalLayout->addLayout(buttonLayout);

	QPushButton* toolDefineGravityButton = new QPushButton("Tool dir is down");
	connect(toolDefineGravityButton, &QPushButton::clicked, this, &TableWidget::onDefineDownWithTool);
	buttonLayout->addWidget(toolDefineGravityButton);

	QPushButton* definePatientAnteriorUpButton = new QPushButton("Patient Anterior Up");
	connect(definePatientAnteriorUpButton, &QPushButton::clicked, this, &TableWidget::onDefinePatientAnteriorUp);
	buttonLayout->addWidget(definePatientAnteriorUpButton);

	QPushButton* definePatientPosteriorUpButton = new QPushButton("Patient Posterior Up");
	connect(definePatientPosteriorUpButton, &QPushButton::clicked, this, &TableWidget::onDefinePatientPosteriorUp);
	buttonLayout->addWidget(definePatientPosteriorUpButton);

	QPushButton* definePatientSuperiorUpButton = new QPushButton("Patient Superior Up");
	connect(definePatientSuperiorUpButton, &QPushButton::clicked, this, &TableWidget::onDefinePatientSuperiorUp);
	buttonLayout->addWidget(definePatientSuperiorUpButton);

	mLabel = new QLabel("Table matrix");
	mVerticalLayout->addWidget(mLabel);
	mMatrixWidget = new Transform3DWidget(this);
	mVerticalLayout->addWidget(mMatrixWidget);
	connect(mMatrixWidget, &Transform3DWidget::changed, this, &TableWidget::matrixWidgetChanged);

	mTableUp = Vector3DProperty::initialize("tableUp", "Up direction",
											"The up direction",
											Vector3D::Zero(),
											DoubleRange(-1, 1, 0.01), 2);
	mVerticalLayout->addWidget(Vector3DWidget::createSmallHorizontal(this, mTableUp));

	mVerticalLayout->addStretch();

	connect(mServices->patient().get(), &PatientModelService::operatingTableChanged,
			this, &TableWidget::backendChanged);
}

void TableWidget::showEvent(QShowEvent* event)
{
	this->backendChanged();
}

/** Called when the moving data in the RegistrationManager has changed.
 *  Update connections.
 */
void TableWidget::backendChanged()
{
	mMatrixWidget->blockSignals(true);

	OperatingTable table = mServices->patient()->getOperatingTable();
//	std::cout << "table " << table.rMtb << std::endl;
	mMatrixWidget->setMatrix(table.rMtb);
	mTableUp->setValue(table.getVectorUp());

	mMatrixWidget->blockSignals(false);
}

/** Called when the matrix in the widget has changed.
 *  Perform registration.
 *
 */
void TableWidget::matrixWidgetChanged()
{
	OperatingTable table = mServices->patient()->getOperatingTable();
	table.rMtb = mMatrixWidget->getMatrix();
	mServices->patient()->setOperatingTable(table);
}

void TableWidget::onDefineDownWithTool()
{
	Transform3D rMt = mServices->spaceProvider()->getActiveToolTipTransform(CoordinateSystem::reference(), true);
	Vector3D toolUp = -Vector3D::UnitZ();
	Vector3D newUp = rMt.vector(toolUp);
	this->setNewUp(newUp);
}
void TableWidget::onDefinePatientAnteriorUp()
{
	this->setNewUp(-Vector3D::UnitY());
}
void TableWidget::onDefinePatientPosteriorUp()
{
	this->setNewUp(Vector3D::UnitY());
}
void TableWidget::onDefinePatientSuperiorUp()
{
	this->setNewUp(Vector3D::UnitZ());
}

void TableWidget::setNewUp(Vector3D newUp)
{
	OperatingTable table = mServices->patient()->getOperatingTable();

	Transform3D R = createTransformRotationBetweenVectors(table.getVectorUp(), newUp);
	table.rMtb = R * table.rMtb;

	mServices->patient()->setOperatingTable(table);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TableTab::TableTab(CoreServicesPtr services, QWidget *parent) :
	PreferenceTab(parent),
	mServices(services)
{

}

void TableTab::init()
{
	TableWidget* tw = new TableWidget(mServices);
	mTopLayout->addWidget(tw);
}


} // namespace cx
