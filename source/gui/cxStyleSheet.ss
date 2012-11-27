/**Qt Style sheet for CustusX. 
 * 
 * Add global styles here.
 */


CXFrame
{
	border: 1px solid gray;
	border-radius: 3px;
}

CXSmallToolButton
{
	border: 1px solid palette(dark);
	border-radius: 4px;
	background-color: palette(button);
	width : 15px;
	height : 15px;
}

CXSmallToolButton::checked
{
	background-color: palette(light)
}

/** This is an attempt to move the QRadioButton vertically in line with the rest of the widgets - no good so far.
 */
/*QRadioButton::indicator
{
	width : 8px;
	height : 8px;
}*/
QRadioButton::indicator
{
	bottom : 2px;
}



/** The Linux GroupBox contains no border. This makes is useless
 *  in cx. New definition here:
 */
QGroupBox
{ 
	font: bold;
	border: 1px solid gray; 
	border-radius: 3px;
	margin-top: 1em;
}
QGroupBox::title 
{
	subcontrol-origin: margin;
	subcontrol-position: top left;
	padding: 0px 0px;
}

		
