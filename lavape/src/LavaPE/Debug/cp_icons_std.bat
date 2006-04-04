if %1==%2 exit
cd %1
if exist %2\LavaIcons echo j | del %2\LavaIcons\*.*
xcopy LavaIcons %2\LavaIcons\
if exist %2\std echo j | del %2\std\*.*
xcopy std %2\std\
copy std_m.htm %2
copy std.htm %2
copy std_1.htm %2
rem if not "%3"=="" if exist %3 echo j | del %3\*.*
