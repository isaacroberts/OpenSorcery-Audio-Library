set OutDir=%1

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy "DemoGainPages.xml"
copy /Y ..\..\Source\DemoGainPages.xml %OutDir%\..\..\Contents\Resources\ > NUL

