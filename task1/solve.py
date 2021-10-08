from struct import *
import sys
sys.setrecursionlimit(2000)
def is_switch(addr): # switch 사용하는함수인지
    while True:
        addr=idc.prev_head(addr)
        if print_insn_mnem(addr)[:7]=="endbr64":
            break
        if print_insn_mnem(addr)[:3]=="add" and get_operand_value(addr,0)==0 and get_operand_value(addr,1)==2:
            return 1
    return 0
def find_correct_value(addr): # 올바른값 구하기
    safe=idc.prev_head(addr)
    if is_switch(addr):
        while True:
            addr=idc.prev_head(addr)
            if print_insn_mnem(addr)=="lea":
                plus=get_operand_value(addr,1)
                break
        addr=safe
        while True:
            addr=idc.prev_head(addr)
            if print_insn_mnem(addr)=="sub":
                minus=get_operand_value(addr,1)
                cmp_value=get_operand_value(idc.next_head(addr),1)
                break
            
        addr=safe
        for i in range(0xff):
            if i-minus<=cmp_value:
                Bytes=unpack("<I",get_bytes(plus+(i-minus)*4,4))[0]+plus&0xffffffff
                if Bytes==safe:
                    return i
        return -1       # 스위치면, 모든 값을 대입해보고 xref인 함수를 찾는다.
                
            
    while True:         #아니면, 비교하는 상수를 찾는다. jz / jnz 케이스로 나눠서 찾음
        addr=idc.prev_head(addr)
        if print_insn_mnem(addr)[:7]=="endbr64":
            break
        if get_operand_value(addr,0)==safe:
            addr=idc.prev_head(addr)
            return get_operand_value(addr,1)
    addr=safe
    if print_insn_mnem(idc.prev_head(addr))[:3]=="jnz":
        return get_operand_value(idc.prev_head(idc.prev_head(addr)),1)
    else:
        return -1       

def find_top(addr): #함수시작구하는 함수.. 인터넷에서 함수를 못찾겠다
    while True:
        if print_insn_mnem(addr)[:7]=="endbr64":
            return addr
        addr=idc.prev_head(addr)
def backtrack(addr,values):     # walk_end 부터 walk_start 까지의 여정을 담은 함수
                                # 모든 뒤에서부터 xref 를 다 돌며 검사하려 했으나
                                # 그냥 첫번째 xref 로만 검사했더니 알맞은 값이 나왔다.
                                # ????... 정확안 알고리즘구현방식을 알고싶다.
    if list(XrefsTo(find_top(addr),flags=0))==[]:
        return
    if find_top(addr) == 0x625B:
        print(''.join(list(map(chr,values[::-1]))))
        return
    for x in XrefsTo(addr,flags=0):
        value=find_correct_value(x.frm)
        if value==-1:
            continue    
        values.append(value)
        backtrack(find_top(x.frm),values)
        break


def main():
    # print(find_correct_value(0x61BF))
    backtrack(0x59BE5,[]) # to 0x625B
if __name__=="__main__":
    main()