Tutorial: Customizing the User Interface {#tutorial_customize_user_interface}
===========================================================

Layouts
-----------------------------------------------------------

Select various layouts that display data in different ways.

1. Change Layout: `Layout->3D AnyDual`.
2. Change Layout: `Layout->3D`.
3. Change Layout: `Layout->3D ACS`.

The layout changes between different modes. The difference between them is how the 2D and 3D views 
are combined to visualize data.
The concept of *View Groups* are important here. Each group of Views show the same data.

As an example, we display two groups containing different data, 3D+ACS and ACS. 
- Group 0 displays A MR volume with an US overlay and segmented vessels. 
- Group 1 displays only the MR volume.

 <table style="width:100%">
  <tr>
    <td> ![3D+ACS, ACS](LayoutExample.png) </td>
    <td> ![Rendering](metastase_mr_us_small.png) </td>
  </tr>
</table> 

In addition to the predefined layouts, custom ones can be created in Layouts->New Layout.


Workflow Steps
-----------------------------------------------------------

The use of CustusX during a surgical procedure can be broken down into a series of steps: 

- Preoperative data acquisition
- Planning
- Registration
- Navigation + Intraoperative data acquisition.
- Postoperative analysis

This is the main workflow. Each step is named a *Workflow Step*. They are controlled from the toolbar, highlighted in the figure.

![Workflow steps](workflow_steps_small.png)

Each step contains its own widget and layout configuration. In addition, the configuration can be customized by the user and 
stored for later use. Use `WorkFlow->Save Desktop` to store, and `WorkFlow->Reset Desktop` to go back to the default for that step.

Preferences
-----------------------------------------------------------

Several features can be fine-tuned in the preferences widget. 

- The *General Tab* controls clinical application and patient files location.
- The *Performance Tab* can save you from a slow system. 
- The *Automation Tab* determine if some actions are performed automatically when given critierions are fulfilled.
- The *Visualization Tab* fine-tune some of the graphics.

