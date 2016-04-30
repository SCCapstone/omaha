@ECHO OFF

SETLOCAL enabledelayedexpansion

FOR %%i IN (*.md) DO (
    SET MDFILE=%%i
    SET FILEBASE=!MDFILE:~0,-3!
    SET HTMLFILE=!FILEBASE!.html
    
    ECHO !HTMLFILE!
    
    ECHO ^<^!!DOCTYPE html^> > !HTMLFILE!
    ECHO ^<html^> >> !HTMLFILE!
    ECHO ^<head^>^<title^>!FILEBASE!^</title^>^</head^> >> !HTMLFILE!
    ECHO ^<body^> >> !HTMLFILE!
    markdown.pl !MDFILE! >> !HTMLFILE!
    ECHO ^</body^> >> !HTMLFILE!
    ECHO ^</html^> >> !HTMLFILE!
)