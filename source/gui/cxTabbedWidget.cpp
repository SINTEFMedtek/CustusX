#include "cxTabbedWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>

namespace cx
{

TabbedWidget::TabbedWidget(QString objectName, QString windowTitle, QWidget* parent) :
    WhatsThisWidget(parent),
    mTabWidget(new QTabWidget(this))
{
  this->setObjectName(objectName);
  this->setWindowTitle(windowTitle);
  this->setWhatsThis(this->defaultWhatsThis());

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mTabWidget);
}

TabbedWidget::~TabbedWidget()
{}

void TabbedWidget::addTab(WhatsThisWidget* newTab, QString newTabName)
{
  int index = mTabWidget->addTab(newTab, newTabName);
  mTabWidget->setTabWhatsThis(index, newTab->defaultWhatsThis());
  newTab->setWhatsThis(newTab->defaultWhatsThis());
}

QString TabbedWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tabbed widget.</h3>"
      "<p>This is a tabbed widget, used for creating a hierarchy of tabbed widgets.</p>"
      "<p><i>If you see this whats this message, something's wrong!</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

RegistrationMethodsWidget::RegistrationMethodsWidget(QString objectName, QString windowTitle, QWidget* parent) :
  TabbedWidget(objectName, windowTitle, parent)
{
  this->setWhatsThis(this->defaultWhatsThis());
}

QString RegistrationMethodsWidget::defaultWhatsThis() const
{
  return"<html>"
      "<h3>Registration methods.</h3>"
      "<p>This is a way to tell the system how to display data and tools in the same world.</p>"
      "<p><i>Choose a method.</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

LandmarkRegistrationsWidget::LandmarkRegistrationsWidget(QString objectName, QString windowTitle, QWidget* parent) :
  TabbedWidget(objectName, windowTitle, parent)
{
  this->setWhatsThis(this->defaultWhatsThis());
}

QString LandmarkRegistrationsWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based registrations.</h3>"
      "<p>This is a set of methods that use landmarks/fiducials to connect the images with the real world.</p>"
      "<p><i>Choose a step to continue..</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------

FastRegistrationsWidget::FastRegistrationsWidget(QString objectName, QString windowTitle, QWidget* parent) :
  TabbedWidget(objectName, windowTitle, parent)
{
  this->setWhatsThis(this->defaultWhatsThis());
}

QString FastRegistrationsWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Fast and approximate registration.</h3>"
      "<p>This is a method designed to be quick and easy, it uses a tools orientation and a landmarks translation to determine where the image is relative to the patient.</p>"
      "<p><i>Choose a step to continue.</i></p>"
      "</html>";
}

//------------------------------------------------------------------------------
} //namespace cx
