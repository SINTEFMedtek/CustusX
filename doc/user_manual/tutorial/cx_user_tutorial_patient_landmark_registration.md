Tutorial: Patient Landmark Registration {#tutorial_patient_landmark_registration}
===========================================================

In order to view a volume in relation to the physical world, the volume and the world must be registered to each other. 
We call this operation Patient Registration.

Patient landmark registration is similar to image landmark registration, but points (landmarks) on one volume are defined in the same location as corresponding points in the physical space.

1. Define landmarks on the fixed volume, the same way as in @ref tutorial_image_landmark_registration.
2. Goto `Image2Patient Tab`.
3. Ensure *Tracking* is on.
4. Use a *Navigation Pointer* to point at the landmarks, then press `Sample Tool` to add them.
5. The system will automatically  update the registration when 3 or more landmarks are defined, i.e. you
   should see that the pointer points to the correct location in the volume.
6. Verify the quality of the registration. A graphical representation of the fixed/patient landmarks and their
   differences give an indication of the match. The accuracy in millimeters are also provided.
