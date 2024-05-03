@echo off
set flatc_path=.\flatc\release\flatc.exe
set target_path=.\fbs\
set output_path=.\fbs\
set dest_path=..\includes\fbs\
set gen_opt=--cpp

for %%f in (%target_path%*.fbs) do (
    echo %flatc_path% %gen_opt% -o %output_path% --cpp-str-flex-ctor %%f
    %flatc_path% %gen_opt% -o %output_path% --cpp-str-flex-ctor %%f
    REM echo %errorlevel%
    if %errorlevel% neq 0 (
        echo Error: Failed for %%f
    ) else (
        echo Successful for %%f
    )
)

echo.
if %errorlevel% neq 0 (
    echo Failed to compile all fbs files.
    cmd /k
) else (
for %%f in (%target_path%*.fbs) do (
    xcopy /Y %%f %dest_path%
    if %ERRORLEVEL% EQU 0 set /a copied_files+=1
)

echo.
echo Number of copied .fbs files: %copied_files%


echo.

REM reset copied_files
set copied_files=0

for %%f in (%target_path%*.h) do (
    xcopy /Y %%f %dest_path%
    if %ERRORLEVEL% EQU 0 set /a copied_files+=1
)
echo.
echo Number of copied .h files:: %copied_files%


cmd /k
)