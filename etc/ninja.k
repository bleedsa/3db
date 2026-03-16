#!/usr/bin/env -S rlwrap etc/k.com

/ utils
has:{~0N~y?x}

/ filters
ext:{x~|#[x]#|y} / y has ext x?
stem:*"."\
cpp:ext".cc"
srcs:{[fil;dir](dir,"/",)'d@&fil'd:0:dir}
libcpp:srcs[cpp;"lib"],srcs[cpp;"lib/fmt"],srcs[cpp;"lib/net"]
bincpp:srcs[cpp;"bin"]
runcpp:srcs[cpp;"run"]
three:srcs[cpp;"3db"]
threei:srcs[cpp;"3i"]

/ config
A:has[;`argv[]]
rel:A["--release"]|A"--rel"
nat:A["--native"]|A"--nat"

/ construct the config dict
cfg:!/+((`name;"3db")
        (`rel;rel)
        (`cxx;$[nat;"clang++";"ape/bin/cosmoc++"]))
cfg,:(!).,'(`cflags;    " "/($[~rel
                               " "/("-g -DDBG=1"
                                    "-fno-omit-frame-pointer")
                               "-O3"]
                             $[~nat;"-mclang";""]
                             "-fshort-enums"
                             "-mno-omit-leaf-frame-pointer -mcosmo"
                             "-Wall -Wextra -Wno-unused-parameter"
                             "-Wno-unused-variable -Wno-unused-function"
                             "-Io/include"))
cfg,:(!).,'(`cxxflags;  " "/(cfg`cflags;"-Iinc";"-std=gnu++23"))
cfg,:(!).,'(`lnkxxflags;"-Lo/lib")

/ ninja
rule:{[n;c]$[`m~@c;[(cmd;dep):(c@)'`cmd`dep / deconstruct
                    "rule ",n,"\n command = ",cmd,"\n depfile = ",dep]
             "rule ",n,"\n command = ",c]}
bld:{[R;out;in]"build ",out,": ",R," "," "/in}
cc:bld"cc"
cxx:bld"cxx"
lnk:bld"lnkxx"

/ files
outof:{"o/",x}
(exeof;objof):{outof y,".",x}@'($[nat;"exe";"com"];"o")
libof:{outof "lib/lib",y,".",x}@"a"
srcof:{outof "src/",x}
bldcxx:{cxx[objof x;,x]}

outd:!/,'(`dep;"$out.d")

/ flags
`0:'{$[x]," = ",$[`C~@q:cfg@x;q;$q]}'!cfg;

/ your basic rules
`0:rule["exe";"$in"]
`0:rule["cxx";outd,(!).,'(`cmd;" "/("$cxx -c $cxxflags -MD -MF $out.d"
                                    "-o $out $in"))]
`0:rule["lnkxx";outd,(!).,'(`cmd;" "/("$cxx $cxxflags -MD -MF $out.d -o $out"
                                      "$in $lnkxxflags"))]
`0:rule["ar";"ape/bin/ar.ape rcs $out $in"]
`0:rule["zip";"zip -r $in"]

/ targets
exe:exeof cfg`name

/ objs
`0:'bldcxx'libcpp;
`0:'bldcxx'bincpp;
`0:'bldcxx'runcpp;
`0:'bldcxx'three;
`0:'bldcxx'threei;

/ out
libout:libof cfg`name
`0:bld["ar";libout;objs:objof'libcpp]
`0:lnk[exe;objs,objof'three]
`0:lnk[int:exeof "3i";objs,objof'threei]
`0:'{lnk[exeof stem x;objs,,x]}'runcpp;
`0:'{lnk[exeof stem x;objs,,x]}'bincpp;
`0:"build cfg: zip o/3db.com run/cfg.ini"
`0:"build test: exe o/bin/test.com "," "/exeof'stem'runcpp
`0:"default cfg "," "/(" "/objs;libout;exe;int;" "/exeof'stem'bincpp);
