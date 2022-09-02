Tutorial: Volumes {#tutorial_volumes}
===========================================================

Viewing a volume
-----------------------------------------------------------
Learn how to load a volume and view it in 2D and 3D.
1. Get a data set in metaheader (mhd) format. You can use the Kaisa data set for a start. 
   (available along with the installation, or at http://custusx.org/uploads/testdata/). See also \ref import_overview
2. Start CustusX
3. Create a new Patient: `File->New Patient` or `<Ctrl+N>`
4. Import a data set into the Patient: `File->Import Data` or `<Ctrl+I>`. Browse to your .mhd file and press Open. 
   This opens the Import widget (\ref import_widget). Enter correct parameters and press Import. The data set is now available in CustusX.
5. Right-click in the 3D View. On the top of the popup menu you should see your imported data set. Select it. 
   The volume will now be displayed in 3D.
6. Press the mouse button and move inside the 3D View. The volume rotates. Pressing Shift at the same time translates it.
7. Right-click in a 2D View and select the volume anew. The volume will now be displayed in all 2D views.
8. Press the mouse button and move inside the 2D view. The yellow cross moves accordingly and let you select where to slice the data.
9. Use the scroll wheel to zoom in 2D and 3D.

Changing Volume Properties
-----------------------------------------------------------
Learn how to change the volume colors, window/level etc.

![Volume Properties Widget](volume_properties_widget.png)

1. Open the *Volume Properties Widget*. It should be available as a tab to the left of the views. Otherwise, it can be found
   by right-clicking on an empty spot on the toolbar on the top of the application window. Select `Properties->Volume` Properties.
2. Open the Transfer Functions tab.
3. On the top is the volume histogram, with the opacity transfer function overlayed. Right-click to add point and then 
   drag them to change the function. 
4. Below the histogram is the color palette bar. The default is black-white. Right-click to add colors, then drag them along the bar.
5. The window/level/alpha/llr are shortcuts. 
1. Window/level defines a window on the intensity range that the colors are clamped onto.
2. Alpha/LLR defines a simple opacity transfer function with cutoff (LLR) and height (Alpha).
6. When you have played with the colors, save your changes by pressing Save. Your preset is now accessible from the drop-down list.

Changing slice properties
-----------------------------------------------------------
Learn how to change the properties of the 2D slice views.
1. Open the *Slice Properties Widget*. It should be available as a tab to left of the views. Otherwise, it can be found 
   by right-clicking on an empty spot on the toolbar on the top of the application. Select `Properties->Slice Properties`. 
2. Open the Color Tab.
3. On the top is the volume histogram, with the opacity transfer function overlayed. The opacity is not relevant if you 
   have only a single volume in a view
4. Below the histogram is the color palette bar. The default is black-white. Right-click to add colors. Then drag them along the bar.
5. The window/level sliders are normally used on 2D slices. The define a window that clamps the color palette. Set them 
   to values that enhance the information in the slice.
6. The values can be saved the same way as in the 3D case.

More on navigation
-----------------------------------------------------------
A few tricks that is nice to know when navigating in volumes:
- The yellow cross visible in the Views are a virtual navigation tool, called the *Manual Tool*. 
  It defines your focus point, and is controlled by the mouse.
- If you get lost, use the `Center Image` button to reset the Tool to the center of the volume. 
  
![Center Image Button](center_image.png)
  
- The right-click menus give a few display options. Try them out.
- The `3D->Right-Click->Show Slice Planes` are useful to see where the 2D views are positioned
  in the 3D View, as shown in the figure below
- In the Volume Properties Widget, you can use the Crop and Clip tools to manipulate the 3D volume.

![The 2D ACS slice planes to the right are indicated by lines in the 3D View](ACS_slice_planes.png)

