@echo off
set dest_server=.\..\TestServer\
set dest_client=.\..\TestClient\
set src=.\packets\

set copied_files=0

for %%f in (%src%*.h) do (
    xcopy /Y %%f %dest_server%
    xcopy /Y %%f %dest_client%
    if %errorlevel% equ 0 set /a copied_files+=1
)
echo Number of copied .h files: %copied_files%
echo.

set copied_files=0

for %%f in (%src%*.cpp) do (
    xcopy /Y %%f %dest_server%
    xcopy /Y %%f %dest_client%
    if %errorlevel% equ 0 set /a copied_files+=1
)
echo Number of copied .cpp files: %copied_files%

cmd /k