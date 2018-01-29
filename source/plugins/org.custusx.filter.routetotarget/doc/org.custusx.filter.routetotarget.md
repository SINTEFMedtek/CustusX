Route-to-Target Filter {#org_custusx_filter_routetotarget}
===================

\addindex routetotarget_filter

Route-to-Target is used in bronchoscopy navigation. It generates a centerline from  the top of trachea to a target position.

Input:
- Target position generated as point metric with \ref utility_widgets_metrics .
- Airway centerline tree generated in the \ref org_custusx_filter_airways widget.

Output:
- Route-to-Target centerline from the top of trachea to the airway centerline position closest to the target.
- Extended route-to-Target. Same as above but extended from airway centerline to target position.

*Algorithm developed by Erlend F. Hofstad.*


\addtogroup cx_user_doc_group_filter

* \ref org_custusx_filter_routetotarget
