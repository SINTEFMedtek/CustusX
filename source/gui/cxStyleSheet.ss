/**Qt Style sheet for CustusX. 
 * 
 * Add global styles here.
 */

/** The Linux GroupBox contains no border. This makes is useless
 *  in cx. New definition here:
 */
QGroupBox
{ 
	font: bold;
	border: 1px solid gray; 
	border-radius: 5px;
	margin-top: 1.5ex;
}
QGroupBox::title 
{
	subcontrol-origin: margin;
	subcontrol-position: top left;
	padding: 0px 0px;
}

		