Tutorial: Video Streaming {#tutorial_video_streaming}
===========================================================

CustusX can display video from an external source using a frame grabber. To achieve this, 
CustusX connects to a grabber application using the OpenIGTLink protocol, which in turn grabs video 
from the frame grabber:

![Video Stream from camera to CustusX](OpenIGTLinkServer_pipeline.png)

This tutorial assumes:
- The Frame Grabber is connected to the CustusX computer.
- The drivers has been properly installed. 
- The Video Source is emitting video.

A quick check is to view the video using a standard webcam-application of some kind, f.ex. Photo Booth 
on Mac or Cheese on Linux. If this works, most of the hardware issues should be OK.

1. Start CustusX, load a Patient or start a new one.
2. Go to the *Acquisition Workflow Step* (the "Acq" arrows to the left on the toolbar).
3. If everything is properly configured, you should now get the video in the *Video View* to the right. Otherwise, read on.
4. Open `Video Connection Widget` .Use default settings, they should be the best choice in most situations.
4. Choose `Connection: Direct Link`. This will run the image server inside CustusX: This is the simplest configuration. 
5. Press `Connect Server` and check the Console Widget for information. The server is now connected to CustusX if 
   the button text changes to "Disconnect Server".
6. If several cameras are connected, choose between them by adding "`--videoport 0`", "`--videoport 1`" and so on in 
   the ` Arguments` box.

MacOS
-----------------------------------------------------------

For the Mac operating system, choose 
1. Connection: Local Server 
2. Local Server: CustusX.app/Contents/MacOS/GrabberServer
3. Press "Connect Server"
4. A server window now appears in the background. If this doesn't work, check the console in the 
   server window and follow instructions.