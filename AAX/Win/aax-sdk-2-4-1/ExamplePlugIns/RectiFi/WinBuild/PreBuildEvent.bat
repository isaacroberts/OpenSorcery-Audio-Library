REM        Copyright 2019 by Avid Technology, Inc.
set OutDir=%1

echo Copy Resources and Create Folder Icon

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy XML files
copy /Y ..\..\*.xml %OutDir%\..\..\Contents\Resources\ > NUL
