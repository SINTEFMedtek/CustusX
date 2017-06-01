Core Video Plugin {#org_custusx_core_video}
===================

Overview {#org_custusx_core_video_overview}
========================

Core video features.



\addindex open_cv_streamer
Video Grabber OpenCV Streamer Service {#org_custusx_core_video_streamer_open_cv}
===========================================================
Stream images from a video grabber.
The video source is installed using the usual method on your operating system, 
then it is made available to the application through the <http://opencv.org/> 
library.



\addindex remote_server_streamer
Remote Server Streamer Service {#org_custusx_core_video_streamer_remote_server}
===========================================================
Receive a video stream and possibly US probe information over an OpenIGTLink connection. 
See <http://openigtlink.org> for more. (The US Probe info is a custom extension).

Enter the IP adress of the remote server. The default port is 18333.

Even though the connection seems successful, you might need to
create an exception for CustusX in your firewall to let the video stream through.



\addindex image_file_streamer
3D Image File Streamer Service {#org_custusx_core_video_streamer_image_file}
===========================================================
Load an image volume (3D) from file, then treat it as a stack of 2D images and 
stream them as video.



\addindex openigtlink_streamer
OpenIGTLink Streamer Service {#org_custusx_core_video_streamer_openigtlink}
===========================================================
ToDo.



\addtogroup cx_user_doc_group_streamer

* \ref org_custusx_core_video_streamer_open_cv
* \ref org_custusx_core_video_streamer_remote_server
* \ref org_custusx_core_video_streamer_image_file
* \ref org_custusx_core_video_streamer_openigtlink
