set OutDir=%1

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy "DemoSamplerPages.xml"
copy /Y ..\..\Source\DemoSamplerPages.xml %OutDir%\..\..\Contents\Resources\ > NUL

