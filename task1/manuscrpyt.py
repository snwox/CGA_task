def decrypt_str(src):
	dec=''
	for c in list(src):
		if c<105 or c > 112:
			if c<114 or c > 121:
				if c < 73 or c > 80:
					if c >=82 and c <=89:
						c-=9
				else:
					c+=9
			else:
				c-=9
		else:
			c+=9
		dec+=chr(c)
	return dec
def get_string(addr):
	out=b''
	while True:
		if get_wide_byte(addr)!=0:
			out+=bytes([get_wide_byte(addr)])
		else:
			break
		addr+=1
	return out

def find_function_arg(addr):
	while True:
		addr=idc.prev_head(addr)
		if print_insn_mnem(addr)[:4]=="push":
			return get_operand_value(addr,0)
def main():
	for x in XrefsTo(0x10003b00,flags=0):
		ref=find_function_arg(x.frm)
		string=get_string(ref)
		decstr=decrypt_str(string)
		print('[STRING]:%s\n[Deobfuscated]:%s' % (string,decstr))
		set_cmt(x.frm,decstr,0)
		set_cmt(ref,decstr,0)

		ct=idaapi.decompile(x.frm)
		tl=idaapi.treeloc_t()
		tl.ea=x.frm
		tl.itp=idaapi.ITP_SEMI
		ct.set_user_cmt(tl,decstr)
		ct.save_user_cmts()

if __name__=='__main__':
	main()


