REM        Copyright 2016 by Avid Technology, Inc.
set OutDir=%1

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy "DemoSynthPages.xml"
copy /Y ..\..\Source\DemoSynthPages.xml %OutDir%\..\..\Contents\Resources\ > NUL

