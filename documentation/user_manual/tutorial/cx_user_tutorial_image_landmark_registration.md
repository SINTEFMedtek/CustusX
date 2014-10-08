Tutorial: Image Landmark Registration {#tutorial_image_landmark_registration}
===========================================================

In order to view two volumes in relation to each other, they have to be image registered. 

Image landmark registration is a simple process where a number of points (landmarks) are defined on the same locations 
in two volumes. The landmarks are then matched by an algorithm, and the volumes are thus registered. The two volumes are 
called fixed and moving in the registration process.

Before performing an image registration, you must determine which landmarks you want to use. They should be clearly
identifiable anatomical points, alternatively artificial fiducials can be attached to the patient prior to image
acquisition. 4 landmarks are the minimum recommended.

Import Data:
-----------------------------------------------------------
1. Load the fixed volume. 
2. Load the moving volume.

Define landmarks on the fixed volume:
-----------------------------------------------------------
3. Goto the *Registration workflow step*
4. Open the `Registration Methods Widget`, `Landmark Tab`, `Image Tab`.
5. The *3D Point Picker Probe* has now been activated. This means that when you click on the volume in 3D, 
   the *Manual Tool* focuses on a point on the volume surface.
6. Make sure that the fixed volume is the Selected Volume in the upper part of the Tab.
7. Press `Add` to store the selection position as a new landmark. Give it a unique name if needed.
8. Repeat step 7 for all landmarks.

![Defining landmarks to the fixed volume](image_registration_1.png)

Define landmarks on the moving volume:
-----------------------------------------------------------
10. Make sure that the moving volume is the Selected Volume in the upper part of the Tab. 
11. Use the Point Picker to define the point in the moving volume.
12. Click on the landmark you want to use, the press `Sample` to store the coordinate information.
    The landmarks now holds two coordinates, one for the moving and one for the fixed volume.
13. Repeat for all landmarks.

Perform Registration
-----------------------------------------------------------
14. Goto the Image2Image Tab.
15. Select Fixed and Moving volumes, press Register. The moving volume now moves towards the fixed volume, giving a
    (if the registration was successful) good overlap.
16. Verify the quality of the registration. 
    Use the `Registration History Widget` to move back and forth in time if you need that.

