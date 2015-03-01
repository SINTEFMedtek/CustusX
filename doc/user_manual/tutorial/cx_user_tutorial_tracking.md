Tutorial: Tracking {#tutorial_tracking}
===========================================================

Connect CustusX to physical tracking devices. NDI devices of the type Polaris and Aurora are supported. 
Here it is assumed that we are setting up Polaris, but the text should apply to Aurora as well.

This tutorial assumes:
- You have selected an appropriate Clinical Application.
- That you have Polaris tracking tools available,
- They are calibrated and defined as tools in the CustusX configuration system.
- NDI drivers are correctly installed (see platform-specific notes on this)
- The Polaris camera is connected to your computer via USB.

Most of the platform-specific issues can be solved by opening the NDI Track application 
(available from <http://www.ndigital.com/>) and verifying that the Tools are tracking 
correctly from within that application. If NDI Track fails, CustusX will certainly fail.

![NDI Polaris Pointer Tool](polaris_tool_small.png)

1. Open Tool Configuration: `CustusX->Preferences` or `<Ctrl+P>`, then select `Tool Configuration`. 
   See the figure for an example.
2. Select the tools you have in the Tools list to the right and drag them over to the Tools list on the left. 
   Set the appropriate filters to see application-specific tools and to chose between Aurora/Polaris.
   In the example, the Intraoperative Navigator and the RefFrame has been selected.

![Tracking Configuration Screen](configure_tracking.png)

3. You might need to check some of the filter buttons for Applications in the upper right to get "your" tools 
   visible in the Tools List.
4. Select a File Name, for example "my_config"
5. Press OK.
6. In the CustusX Main Window, start tracking: `Tracking->Start Tracking`.
7. After a while, your tools should be visible as buttons on to status bar on the bottom of the application
      
  1. No buttons: Not configured. Something went wrong in the Tool Configure Panel.
  2. Gray: CustusX has configured the tools, but they are not connected to the tracking hardware. 
  3. Red: OK but not visible. Try to place the tools in the camera field of view.
  4. Green: OK and visible.
  5. Light Green: As Green. This is the active tool, controlling the navigation.
8. You should now be able to navigate using the tools. 
   In order to navigate correctly within preoperative data, a *Patient Registration* must be performed. 

![Gray buttons: Configured but unconnected to hardware](tracking_tool_buttons_configured.png)
   