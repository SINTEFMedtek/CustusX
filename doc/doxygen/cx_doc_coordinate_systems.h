/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


namespace ssc
{

/**\page ssc_page_coords Coordinate Systems
 *
 * This page describes the CustusX way of handling coordinate systems. A basic
 * knowledge of linear algebra and geometry is assumed. A coordinate system
 * can also be called a space or frame. Space will be preferred here.
 *
 * A space always has an ID. It is defined in terms of one other space,
 * called its parent space. The origin and coordinate axes of the space is
 * defined in the parent space. The origin and coordinate axes are concatenated
 * into a 4x4 affine matrix. Spaces that are connected through a chain of
 * references are connected. Otherwise they are unconnected and need a
 * registration in order to be connected.
 *
 *
 *
 *
 * \section ssc_section_spaces_notation Notation
 *
 * A point or vector \p p in space \p q are denoted
 *
 * <tt> p_q </tt>
 *
 * Space \p q has a parent space \p r. The affine matrix, or transform, \p M relating
 * the two spaces are denoted
 *
 * <tt> q_M_r </tt> (sometimes simply \p qMr)
 *
 * \p q_M_r is defined such that a point p_r in the parent space premultiplied
 * with \p q_M_r yields the same point \p p_q represented in space \p q:
 *
 * <tt> p_q = q_M_r * p_r </tt>
 *
 * The multiplication here is done using homogenous coordinates, where a point
 * is [x,y,z,1] and a vector is [x,y,z,0].
 *
 *
 *
 *
 * \section ssc_section_spaces_implementation Implementation
 *
 * Points and vectors are implemented using Vector3D. Vector3D only contains
 * [x,y,z], thus it is context-dependent if it is a vector or point, and also
 * which space it is defined in.
 *
 * Transformations (the matrix \p qMr) are implemented using Transform3D. The
 * internal representation is a 4x4 matrix. Transform3D contains helper methods
 * vec(Vector3D) and coord(Vector3D) that transforms the vector or point,
 * respectively.
 *
 * The class CoordinateSystemHelpers provides some nice utility functions.
 *
 *
 *
 *
 * \section ssc_section_specific_spaces Specific Spaces
 *
 * \image html ssc_coordinate_systems.png "Major Coordinate systems"
 *
 * The figure and table shows the most important spaces in CustusX. Some spaces
 * have a subscript; This means that there are several of them.
 *
<TABLE>
	<TR>
		<TH>Name</TH>
		<TH>ID</TH>
		<TH>Description</TH>
	</TR>
	<TR>
		<TD>reference</TD>
		<TD>r</TD>
		<TD>The basic space that connects everything. Has no parent space.</TD>
	</TR>
	<TR>
		<TD>patient reference</TD>
		<TD>pr</TD>
		<TD>A physical space, usually the reference tool. When rMpr is defined, the system is said to be patient registered, meaning image spaces are connected to the navigation tools.</TD>
	</TR>
	<TR>
		<TD>data i</TD>
		<TD>d_i</TD>
		<TD>Each Data (Image or Mesh or other) has an associated space d_i. In simple cases, it has the reference space as its parent space, but it might also have another data space as parent space. This gives the ability to describe a tree of data spaces. The transform rMd_i is always stored in Data, regardless of the value of the parent space.</TD>
	</TR>
	<TR>
		<TD>slice k</TD>
		<TD>s_k</TD>
		<TD>During slicing (for display in a 2D View) a space s is defined. The xy-plane of this space is the slice plane. It is defined with r as the parent space.</TD>
	</TR>
	<TR>
		<TD>tool j</TD>
		<TD>t_j</TD>
		<TD>Each Tool has a space t that defines its position. The origin is usually the tool tip, this may vary for ultrasound probes.</TD>
	</TR>
	<TR>
		<TD>tool offset j</TD>
		<TD>to_j</TD>
		<TD>Tools can have a virtual offset along its local z axis. The space thus translated is denoted to.</TD>
	</TR>
	<TR>
		<TD>sensor j</TD>
		<TD>s_j</TD>
		<TD>Tools usually have a tracking sensor attached that communicates with the tracking system. The local sensor space is denoted s. The transform sMt is the Tool calibration. (Yes, we have two spaces called s. We have to live with that, but they are seldom seen in the same spot)</TD>
	</TR>
</TABLE>
 *
 * The following figure shows a case where some Data has a parent space
 * different from the reference space. One MR volume exists, and some vascular
 * structures are segmented out from that volume and imported as a separate
 * Data object (a Mesh). For consistency, the new Data have the MR as the
 * parent frame.
 *
 * The CT volume is imported separately, but is at some point image-registered
 * to the MR volume. In the process, the parent frame is changed to the MR
 * volume.
 *
 * \image html ssc_coords_data.png "Example of data spaces with parent space different from the reference space."
 *
 */

} // namespace ssc

