/**Qt Style sheet for CustusX. 
 * 
 * Add global styles here.
 */


CXFrame
{
	border: 1px solid gray;
	border-radius: 3px;
}


CXFrame#FilterBackground
{
	border: 0px solid gray;
	border-radius: 3px;
	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
									  stop: 0 palette(midlight), stop: 1 palette(dark));
}
/*background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
								  stop: 0 #f6f7fa, stop: 1 #dadbde);
*/

CXSmallToolButton
{
	border: 1px solid palette(dark);
	border-radius: 4px;
	background-color: palette(button);
	width : 15px;
	height : 15px;
}

CXSmallToolButton#RunFilterButton
{
	width : 60px;
	height : 15px;
	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
									  stop: 0 palette(midlight), stop: 1 palette(dark));
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
 /*
 disabled: OSX10.8 groupbox was cool. Investigate.
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
*/
		
