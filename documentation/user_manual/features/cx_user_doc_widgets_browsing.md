Browsing Widgets {#browsing_widgets}
===========================================================

[TOC]

\addindex browsing_widgets_frame_tree
Frame Tree Widget {#browsing_widgets_frame_tree}
===========================================================

Lets you look at the frame dependencies between different data.




\addindex property_widgbrowsing_widgets_playbackets_volume
Playback Widget {#browsing_widgets_playback}
===========================================================

View the previously recorded tool movements and video from the current patient.
(video is only recorded during 3D acquisition).

Hover the mouse above the timeline in order to get information about that time. 

- Green indicates visible tools.
- Orange indicates recorded video.
- Black rectangles denote events, such as registrations and data imports.

> **Tip:** Press the red putton to start playback.

> **Note:** The tools in the current configuration must match those stored in the 
> patient you want to playback. It there is a mismatch, a notification will appear
> in the console. Use that to add the correct tools.




\addindex browsing_widgets_registration_history
Registration History Widget {#browsing_widgets_registration_history}
===========================================================

Use the registration history to rewind the system to previous time. When history is rewinded, 
all registrations performed after the active time is ignored by the system.

> **NB:** While a previous time is active, *no new registrations or adding of data* should be performed. 
> This will lead to undefined behaviour!
