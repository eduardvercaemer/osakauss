import re
with open('Character.h','w') as h:
	h.write('')
with open('Character.h',"ab") as f:
	f.write(b"const char[128] = {")
	for x in [chr(i) for i in range(128)]:
		BYTE = ord(x)
		f.write( bytes(r"'" + fr"{BYTE}" + r"'"+ ",\n","utf-8") )
	f.write(b"}")
