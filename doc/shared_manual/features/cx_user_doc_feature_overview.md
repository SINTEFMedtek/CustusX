Feature Overview {#feature_overview}
===========================================================
\addindex main_window

Main Window {#feature_overview_main_window}
===========================================================

![CustusX Main Window](mainwindow.png)

The main window present the main features of CustusX. The View Area display visualizations of
volumes and models, and is the most important part of the system. Everything else is support.
The Widgets are windows that contain functionality of all kinds. They are usually placed to
the left, but can be dragged to all sides if needed. The Toolbar on top contains shortcuts
to important functions. The most important is the Workflow Buttons, which can be used to change
the main window to a specific state. The Menu (not shown in the image) also contains important 
functions. 
 
View Layout {#feature_overview_view_layouts}
-----------------------------------------------------------

The view area consist of several Views, each of which can visualize either 2D, 3D or video data. 
The 2D views show data in either Saggital, Axial, Coronal or in a plane defined by the active tool.
Each View belong to a View Group, and all Views in a View Group display the *same data*.

As an example, we display two groups containing different data, 3D+ACS and ACS. 
- Group 0 displays A MR volume with an US overlay and segmented vessels. 
- Group 1 displays only the MR volume.

 <table style="width:100%">
  <tr>
    <td> ![3D+ACS, ACS](LayoutExample.png) </td>
    <td> ![Rendering](metastase_mr_us_small.png) </td>
  </tr>
</table> 

Right-click on a view to control which data are visualized there.

Change layout from `Menu->Layouts`. In addition to the predefined layouts, custom ones can be created 
in Layouts->New Layout.

It is possible to visualize overlayed volumes in 2D. Use the Slice Properties Widget to control this fully.


Widgets {#feature_overview_widgets}
-----------------------------------------------------------

Widgets contains the bulk of functionality in CustusX, and more can be added by the use of \ref plugins.
Show a Widget by selecting `Menu->Window`, or simply right-clicking on the title bar of an existing
Widget.

All specific widgets are described here:
- \subpage property_widgets
- \subpage utility_widgets
- \subpage browsing_widgets
- \subpage algorithms_widgets
- See \ref plugins for more widgets

Workflow Steps {#feature_overview_workflow_steps}
-----------------------------------------------------------

The use of CustusX during a surgical procedure can be broken down into a series of steps: 

- Preoperative data acquisition
- Planning
- Registration
- Navigation + Intraoperative data acquisition.
- Postoperative analysis

This is the main workflow. Each step is named a *Workflow Step*. They are controlled from the toolbar:

![Workflow steps](workflow_steps.png)

Each step contains its own widget and layout configuration, i.e. the step stores the setup that you have
on screen. In addition, the configuration can be customized by the user and stored for later use. 
Use `WorkFlow->Save Desktop` to store, and `WorkFlow->Reset Desktop` to go back to the default for that step.

![Workflow steps](workflow_store.png)


Preferences {#feature_overview_preferences}
-----------------------------------------------------------

Several features can be fine-tuned in the preferences widget. 

- The *General Tab* controls clinical application and patient files location.
- The *Performance Tab* can save you from a slow system. 
- The *Automation Tab* determine if some actions are performed automatically when given critierions are fulfilled.
- The *Visualization Tab* fine-tune some of the graphics.
- The *Tool Configuration Tab* let you configure tracking tools.

Other core features
-----------------------------------------------------------
- \subpage org_custusx_resource_filter
- \subpage import_overview
- \subpage export_overview

