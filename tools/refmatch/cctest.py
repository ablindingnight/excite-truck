# Usage: python cctest.py <unit path w/o ext, e.g. Runtime.PPCEABI.H/NMWException> <src ext> <extra flags...>
import subprocess, sys, os, re, shutil, tempfile
root = os.getcwd()
unit, ext = sys.argv[1], sys.argv[2]
extra = sys.argv[3:]
base = ["-nodefaults","-proc","gekko","-align","powerpc","-enum","int","-fp","hardware","-O4,p","-inline","auto",
        "-pragma","cats off","-pragma","warn_notinlined off","-maxerrors","1","-nosyspath","-RTTI","off","-fp_contract","on",
        "-multibyte","-i","include","-DNDEBUG=1","-use_lmw_stmw","on","-str","reuse,pool,readonly","-gccinc","-common","off"]
versions = ["GC/2.7","GC/3.0a3","GC/3.0a3.2","GC/3.0a3.3","GC/3.0a3.4","GC/3.0a5","GC/3.0a5.2","Wii/1.0RC1","Wii/1.0a","Wii/1.0","Wii/1.1"]
tmp = tempfile.mkdtemp()
dtk = os.path.join(root,"build","tools","dtk.exe")
def disasm(obj, out):
    subprocess.run([dtk,"elf","disasm",obj,out],capture_output=True)
    lines=[]
    for f in sorted(os.listdir(out)) if os.path.isdir(out) else [out]:
        p=os.path.join(out,f) if os.path.isdir(out) else out
        for l in open(p,errors="ignore"):
            m=re.match(r"/\* [0-9A-F]+ [0-9A-F]+ +[0-9A-F ]{11} \*/\s+(.*)",l.strip())
            if m: lines.append(re.sub(r"\s+"," ",m.group(1)))
    return lines
target = disasm(os.path.join(root,"build","REXE01","obj",unit+".o"), os.path.join(tmp,"t.s"))
for v in versions:
    obj=os.path.join(tmp,v.replace("/","_")+".o")
    r=subprocess.run([os.path.join(root,"build","compilers",*v.split("/"),"mwcceppc.exe"),*base,*extra,"-c",os.path.join(root,"src",unit+ext),"-o",obj],capture_output=True,text=True)
    if r.returncode: print(f"{v:12} COMPILE ERROR {r.stdout[-300:]}{r.stderr[-300:]}"); continue
    mine=disasm(obj,os.path.join(tmp,v.replace("/","_")+".s"))
    diff=sum(1 for a,b in zip(mine,target) if a!=b)+abs(len(mine)-len(target))
    if os.environ.get("SHOW")==v:
        for a,b in zip(mine,target):
            if a!=b: print("  T:",b,"| M:",a)
    print(f"{v:12} insns {len(mine)}/{len(target)}  mismatched {diff}")
