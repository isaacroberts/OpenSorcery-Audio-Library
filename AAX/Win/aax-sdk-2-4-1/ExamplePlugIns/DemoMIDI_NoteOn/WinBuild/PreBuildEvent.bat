set OutDir=%1

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy "DemoMIDIPages.xml"
copy /Y ..\..\Source\DemoMIDIPages.xml %OutDir%\..\..\Contents\Resources\ > NUL

