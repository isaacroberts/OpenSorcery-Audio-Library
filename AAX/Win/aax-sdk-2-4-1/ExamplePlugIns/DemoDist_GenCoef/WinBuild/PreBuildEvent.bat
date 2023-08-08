set OutDir=%1

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy "DemoDist.xml"
copy /Y ..\..\Source\DemoDist.xml %OutDir%\..\..\Contents\Resources\ > NUL

