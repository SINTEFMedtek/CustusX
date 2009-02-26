import sys
from struct import *

#------
#tool_fname = "01-076-0179_Planning-Navigator.tf"
#tool_fname = "02-077-0191_Sterile-RefFrame.tf"
#tool_fname = "03-078-0199_US-probe.tf"
#tool_fname = "04-062-0142_Large-Custom-Navigator.tf"
#tool_fname = "05-080-0214_Small-Custom-Navigator.tf"
#tool_fname = "07-081-0223_Intraoperative-Navigator.tf"
#tool_fname = "32-060-0263_10-MHz-FPA.tf"

#------
print "==========================================="
print "Syntax:	python sys2.py <tool_file_name>"
print "-------------------------------------------"

tool_file_name = sys.argv[1]
print "tool_file_name: ", tool_file_name

rom_file_name = tool_file_name.replace( ".tf", ".rom" )
print "rom_file_name:  ", rom_file_name

cal_file_name = tool_file_name.replace( ".tf", ".cal" )
print "cal_file_name:  ", cal_file_name

print "-------------------------------------------"

#------
tool_file = file( tool_file_name, "r" )
rom_file = file( rom_file_name, "w" )
cal_file = file( cal_file_name, "w" )

#------
s = tool_file.read(23)
print s
print "-------------------------------------------"

tag = tool_file.read(4)
print tag
#tag = unpack('I', tag)
tag = unpack('>I', tag)
tag = tag[0]

while tag < 14:
	size = tool_file.read(4)
#	size = unpack('I',size)
	size = unpack('>I',size)
	size = size[0]
	
	block = tool_file.read(size)

	#1 / U16 = 2 (Polaris)
	if (tag == 1) or (tag == 4) or (tag == 6):
#		block = unpack('H', block)
		block = unpack('>H', block)
		block = block[0]
	elif tag == 8:
		asize = block[:8]
#		asize = unpack('2L', asize)
		asize = unpack('>2L', asize)
		m = block[8:128+8]
#		m = unpack('16d', m)
		m = unpack('>16d', m)
		block = m
		line1 = '%12.8f %12.8f %12.8f %16.8f\n' % (m[0], m[1], m[2], m[3])
		line2 = '%12.8f %12.8f %12.8f %16.8f\n' % (m[4], m[5], m[6], m[7])
		line3 = '%12.8f %12.8f %12.8f %16.8f\n' % (m[8], m[9], m[10], m[11])
		line4 = '%12.8f %12.8f %12.8f %16.8f\n' % (m[12], m[13], m[14], m[15])
		cal_file.write( line1 + line2 + line3 + line4 )
	elif tag == 10:
		rom_file.write(block)
		block = 'To long'
	elif tag == 12:
		block = 'To long'

	print( (tag, size, block) )

	tag = tool_file.read(4)
#	tag = unpack('I', tag)
	tag = unpack('>I', tag)
	tag = tag[0]


tool_file.close()
rom_file.close()
cal_file.close()

