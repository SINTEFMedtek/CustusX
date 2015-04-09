Architecture {#architecture}
===========================================================

Overview {#architecture_overview}
===========================================================

CustusX uses a layered architecture, with external libraries at the bottom, 
a resource layer consisting of reusable components and classes, a plugin layer 
for handling of extensibility, modularity and state, a logic layer for system 
management, and a top layer proving the user interface.


External libraries {#architecture_external_libraries}
-----------------------------------------------------------
The Qt framework is used throughout the application, at the same level as the 
C++ libraries. Qt greatly simplifies C++ development through its signals and 
slots mechanism and extensive libraries. VTK and ITK are used for visualization 
and image processing, CTK for plugins and DICOM, Eigen for basic math, while 
DCMTK, OpenCV, OpenIGTLink and IGSTK are used for specialized operations.

CustusX uses a \ref dev_superbuild for handling many of the external libraries.


Resource Layer {#architecture_resource_layer}
-----------------------------------------------------------
A collection of common utility classes available to everyone.

The Resource Layer consists of building blocks that are available to everyone.
Everything here is/should be reusable, i.e. used by at least two other modules.

The Core library is the basic component, all other libraries depend on that
and are specialized in some way.

In principle, this layer contains no global state, but a few exceptions exist:
* cx::Reporter
* cx::Settings
* cx::XmlOptionFile



Plugin Framework {#architecture_plugin_framework}
-----------------------------------------------------------

CustusX use the OSGi-based [CTK Plugin Framework](http://www.commontk.org)  as 
the basis for the plugin part of the architecture. This framework introduces 
two central concepts: Services and plugins (OSGi uses the name bundle instead 
of plugin). Services are abstract interfaces that are available from the 
central plugin framework, which is implemented as a singleton software pattern. 
Service consumers are unaware of the service providers. Services may also 
appear and disappear at any time. Plugins are physical components (shared 
libraries) that can be loaded and unloaded at runtime. Each plugin provides one 
or more service implementations, while they also can act as service consumers.

### Core Services

An IGT platform requires some core services in order to operate. These are 
declared as interfaces in the resource layer, thus making them available to 
everyone. They are implemented as plugins that always are included with the 
system, and it is assumed to be one instance of each:

|                           |                   |
| --------------------------|-------------------|
| cx::SessionStorageService | Persistent storage of sessions, corresponding to documents in other applications.
| cx::PatientModelService   | Represents the virtual patient, containing image data, geometric models, and spatial and temporal relations between these.
| cx::TrackingService       | Access to physical and simulated navigation tracking systems.
| cx::VideoService          | Access to video sources, such as endoscopes and ultrasound probes.
| cx::ViewService           | Framework for displaying the geometric data in a 3D scene. Several 2D and 3D views visualize the data from various directions and combinations.

Some services are not widely used. The interfaces are declared inside each 
plugin, thus requiring plugins to depend on them. It is assumed to be zero or 
one instance of each:
|                             |                   |
|-----------------------------|-------------------|
| cx::RegistrationService     | Perform Image-to-Patient registration and Image-to-Image registration.
| cx::AcquisitionService      | Record Video and Tracking data for later use.
| cx::UsReconstructionService | Create a 3D volume from a sequence of US images with tracking information.


### Extension Points

Services can also be used as extension points, i.e. adding more instances of 
the same service adds more functionality to the system. Currently the following 
extension points are available:

|                                 |                   |
|---------------------------------|-------------------|
|cx::GUIExtenderService           | Provide a collection of widgets and toolbars that can be added to for example the cx::MainWindow. This is the most generic extension point and can be used for anything that can be accessed from a GUI.
|cx::RegistrationMethodService    | Defines an Image-Patient or Image-Image registration method that is made available through the cx::RegistrationService.
|cx::ReconstructionMethodService  | Defines a US reconstruction method that is made available through the cx::UsReconstructionService.
|cx::StreamerService              | Support for a video source or an ultrasound scanner. Used by the cx::VideoService.
|cx::Filter                       | Defines an algorithm that can be applied to volume or geometric data.


Logic Layer
-----------------------------------------------------------
The \ref cx_logic, with cx::LogicManager as the main class, manages the plugin 
framework and application lifecycle.

GUI Library
-----------------------------------------------------------
The \ref cx_gui contains user interface components that are available to the 
application, and works on top of the \ref cx_logic. That means that the library 
has access to all installed plugins and the \ref cx_resource.

Application Layer
-----------------------------------------------------------
The application layer mostly consist of a single main() function which calls 
the cx::LogicManager and GUI. The default application is CustusX, which uses 
cx::MainWindow as frontend. See \ref customize_applications for how to create your 
own applications.

