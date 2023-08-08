GOTO EndCopyright
 *
 * Copyright 2013 by Avid Technology, Inc.
 *
 :EndCopyright

set OutDir=%1

CALL ..\..\..\..\Utilities\CreatePackage.bat %OutDir% ..\..\..\..\Utilities\PlugIn.ico

echo Copy "DemoDelayPages.xml"
copy /Y ..\..\Source\DemoDelayPages.xml %OutDir%\..\..\Contents\Resources\ > NUL

