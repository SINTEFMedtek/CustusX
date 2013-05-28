#include "cxBaseWidget.h"

namespace cx
{

BaseWidget::BaseWidget(QWidget* parent, QString objectName, QString windowTitle) :
    OptimizedUpdateWidget(parent), mObjectName(objectName), mWindowTitle(windowTitle)
{
    if (mObjectName=="US Reconstruction")
        std::cout << ":: [" <<  this->objectName().toAscii().constData() << "]" << std::endl;
    this->setObjectName(mObjectName);
    this->setWindowTitle(mWindowTitle);
}

QWidget* BaseWidget::createMethodWidget(QWidget* inputWidget, QWidget* outputWidget, QString methodname, bool inputChecked, bool outputVisible)
{
    QWidget* retval = new QWidget(this);
    QVBoxLayout* toplayout = new QVBoxLayout(retval);
    QGridLayout* layout = new QGridLayout();
    toplayout->addLayout(layout);
    toplayout->addStretch();

    QLabel* methodLabel = new QLabel("<b>" + methodname + "</b>");
    QCheckBox* checkBox = new QCheckBox("generate");

    inputWidget->setVisible(inputChecked);
    outputWidget->setVisible(outputVisible);
    connect(checkBox, SIGNAL(clicked(bool)), inputWidget, SLOT(setVisible(bool)));

    layout->addWidget(methodLabel, 0, 0);
    layout->addWidget(checkBox, 0, 1);
    layout->addWidget(inputWidget, 1, 0, 1, 2);
    layout->addWidget(outputWidget, 2, 0, 1, 2);

    return retval;
}

QGroupBox* BaseWidget::createGroupbox(QWidget* widget, QString boxname)
{
    QGroupBox* retval = new QGroupBox(this);
    QVBoxLayout* toplayout = new QVBoxLayout(retval);

    QLabel* nameLabel = new QLabel(boxname);
    toplayout->addWidget(nameLabel);
    toplayout->addWidget(widget);

    return retval;
}

QFrame* BaseWidget::createHorizontalLine() ///< creates a horizontal line witch can be inserted into widgets
{
    QFrame* retval = new QFrame();
    retval->setFrameStyle(QFrame::Sunken + QFrame::HLine);
    return retval;
}

CXFrame* BaseWidget::wrapInFrame(QWidget* base)
{
    CXFrame* frame = new CXFrame(NULL);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    layout->addWidget(base);
    return frame;
}

QGroupBox* BaseWidget::wrapInGroupBox(QWidget* base, QString name)
{
    QGroupBox* groupBox = new QGroupBox(name);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setMargin(2);
    layout->addWidget(base);
    return groupBox;
}

void BaseWidget::adjustSizeSlot()
{
    this->parentWidget()->adjustSize();
    this->adjustSize();
}

void BaseWidget::showEvent(QShowEvent* event)
{
    this->setWhatsThis(this->defaultWhatsThis());
    QWidget::showEvent(event);
}


}

