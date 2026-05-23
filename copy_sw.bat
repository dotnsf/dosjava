@echo off
echo Copying switch test files to DOSBox-X...

copy build\bin\djc.exe C:\dosbox\dosjava\djc.exe
copy build\bin\djvm.exe C:\dosbox\dosjava\djvm.exe

copy tests\swsimple.jav C:\dosbox\dosjava\swsimple.jav
copy tests\swint.jav C:\dosbox\dosjava\swint.jav
copy tests\swlong.jav C:\dosbox\dosjava\swlong.jav
copy tests\swstring.jav C:\dosbox\dosjava\swstring.jav
copy tests\swdef.jav C:\dosbox\dosjava\swdef.jav
copy tests\swnest.jav C:\dosbox\dosjava\swnest.jav
copy tests\swmany.jav C:\dosbox\dosjava\swmany.jav
copy tests\swfall.jav C:\dosbox\dosjava\swfall.jav

copy tswall.bat C:\dosbox\dosjava\tswall.bat

echo.
echo Files copied successfully!
echo Run tswall.bat in DOSBox-X to test all switch features.