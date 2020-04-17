import subprocess
from collections import Counter

subprocess.call(' ../pin/pin -ifeellucky -t obj-intel64/calltrace_cust.so -- ../test/ptrace ;',shell=True)
print(">>>>>>>>>>>>>>>>>>>>>>>>>>>> Binary excecuted <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
fo = open("calltrace.out","r")
fin=[]

for i in fo.readlines():
    if(i[0]!='_' and i[0]!='.' and i[0]!="$" and i[0]!="(" and i[0]!="#" and i[0]!="@" and ("Address" not in i)):
        i = i.replace('\n','')
        fin.append(i)
        if(i == "ptrace@plt"):
            print("PTRACE detected")

print(">>>>>>>>>>>>>>>>>>>>>>>>>>>> Stats are as follows <<<<<<<<<<<<<<<<<<<<<<<<<<")

for i in range(len(Counter(fin).keys())):
    print("Number of times "+Counter(fin).keys()[i]+" was called: "+str(Counter(fin).values()[i]))
    