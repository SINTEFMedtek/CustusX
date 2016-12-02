#include "cxOperatingTableWidget.h"
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

OperatingTableWidget::OperatingTableWidget(CoreServicesPtr services, QWidget* parent) :
	QWidget(parent),
	mServices(services)
{
	mVerticalLayout = new QVBoxLayout(this);

	mVerticalLayout->setMargin(0);

	mVerticalLayout->addWidget(new QLabel("Define the operating table up direction. See the preferences help page for more information."));

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mVerticalLayout->addLayout(buttonLayout);

	QPushButton* toolDefineGravityButton = new QPushButton("Tool direction is down");
	connect(toolDefineGravityButton, &QPushButton::clicked, this, &OperatingTableWidget::onDefineDownWithTool);
	buttonLayout->addWidget(toolDefineGravityButton);

	QPushButton* definePatientAnteriorUpButton = new QPushButton("Patient Anterior Up");
	connect(definePatientAnteriorUpButton, &QPushButton::clicked, this, &OperatingTableWidget::onDefinePatientAnteriorUp);
	buttonLayout->addWidget(definePatientAnteriorUpButton);

	QPushButton* definePatientPosteriorUpButton = new QPushButton("Patient Posterior Up");
	connect(definePatientPosteriorUpButton, &QPushButton::clicked, this, &OperatingTableWidget::onDefinePatientPosteriorUp);
	buttonLayout->addWidget(definePatientPosteriorUpButton);

	QPushButton* definePatientSuperiorUpButton = new QPushButton("Patient Superior Up");
	connect(definePatientSuperiorUpButton, &QPushButton::clicked, this, &OperatingTableWidget::onDefinePatientSuperiorUp);
	buttonLayout->addWidget(definePatientSuperiorUpButton);

	mLabel = new QLabel("Table transformation matrix, rMot:");
	mVerticalLayout->addWidget(mLabel);
	mMatrixWidget = new Transform3DWidget(this);
	mVerticalLayout->addWidget(mMatrixWidget);
	connect(mMatrixWidget, &Transform3DWidget::changed, this, &OperatingTableWidget::matrixWidgetChanged);

	mTableUp = Vector3DProperty::initialize("tableUp", "Up direction",
											"The up direction",
											Vector3D::Zero(),
											DoubleRange(-1, 1, 0.01), 2);
	mVerticalLayout->addWidget(Vector3DWidget::createSmallHorizontal(this, mTableUp));

	mVerticalLayout->addStretch();

	connect(mServices->patient().get(), &PatientModelService::operatingTableChanged,
			this, &OperatingTableWidget::backendChanged);
}

void OperatingTableWidget::showEvent(QShowEvent* event)
{
	this->backendChanged();
}

/** Called when the moving data in the RegistrationManager has changed.
 *  Update connections.
 */
void OperatingTableWidget::backendChanged()
{
	mMatrixWidget->blockSignals(true);

	OperatingTable table = mServices->patient()->getOperatingTable();
	mMatrixWidget->setMatrix(table.rMot);
	mTableUp->setValue(table.getVectorUp());

	mMatrixWidget->blockSignals(false);
}

/** Called when the matrix in the widget has changed.
 *  Perform registration.
 *
 */
void OperatingTableWidget::matrixWidgetChanged()
{
	OperatingTable table = mServices->patient()->getOperatingTable();
	table.rMot = mMatrixWidget->getMatrix();
	mServices->patient()->setOperatingTable(table);
}

void OperatingTableWidget::onDefineDownWithTool()
{
	Transform3D rMt = mServices->spaceProvider()->getActiveToolTipTransform(CoordinateSystem::reference(), true);
	Vector3D toolUp = -Vector3D::UnitZ();
	Vector3D newUp = rMt.vector(toolUp);
	this->setNewUp(newUp);
}
void OperatingTableWidget::onDefinePatientAnteriorUp()
{
	this->setNewUp(-Vector3D::UnitY());
}
void OperatingTableWidget::onDefinePatientPosteriorUp()
{
	this->setNewUp(Vector3D::UnitY());
}
void OperatingTableWidget::onDefinePatientSuperiorUp()
{
	this->setNewUp(Vector3D::UnitZ());
}

void OperatingTableWidget::setNewUp(Vector3D newUp)
{
	OperatingTable table = mServices->patient()->getOperatingTable();

	Transform3D R = createTransformRotationBetweenVectors(table.getVectorUp(), newUp);
	table.rMot = R * table.rMot;

	mServices->patient()->setOperatingTable(table);
}


} // namespace cx
