=============================================

AAX SDK 2.4.1

ReadMe.txt
Copyright 2014-2022 by Avid Technology, Inc.
All rights reserved.

CONFIDENTIAL: This document contains confidential information.  Do not
read or examine this document unless you are an Avid Technology employee
or have signed a non-disclosure agreement with Avid Technology which protects
the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION
CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT
OF Avid Technology, INC.

=============================================

Welcome to AAX! Open the SDK's HTML documentation for complete documentation.

---
Quick Start

  1) Open the AAX Library project for your platform and IDE from Libs/AAXLibrary
       or
     Open the AAX_SDK Xcode workspace from the xcode folder or the Visual Studio
     solution from the msvc folder. This workspace/solution contains the AAX
     Library project as well as all SDK example projects.

     The AAX Library is a static library containing base implementations of the
     AAX API. AAX plug-ins typically link to this library to inherit a default
     implementation of the AAX API, then override the library classes to provide
     customized behavior.

  2) Compile the AAX Library using your desired build configuration

  3) Open and compile the DemoGain example plug-in at ExamplePlugIns/DemoGain

  4) Install the resulting DemoGain_example.aaxplugin into the system AAX plug-ins
     directory

     Mac: /Library/Application Support/Avid/Audio/Plug-Ins
     Windows: C:\Program Files\Common Files\Avid\Audio\Plug-Ins

     Alternatively, you can create a folder named "Plug-Ins" next to your host
     executable. This plug-ins folder will be used instead of the the system
     plug-ins folder.

  5) Run the plug-in in an AAX host which supports unsigned AAX plug-ins, such as a
     Pro Tools developer build, Media Composer, or the DSH command-line shell

---
Usage Notes

* On Windows, if you place the SDK too deep in your file hierarchy you may
  encounter build failures due to the system's path length limitation. To avoid
  this issue, locate the SDK near the root of your drive.

---
Compatibility

This release of the AAX SDK has been tested with the following IDEs

  Visual Studio
    Visual Studio 2013
    Visual Studio 2017 (see notes below)

  Xcode
    Xcode 8.3.3
    Xcode 10.1

  TI Code Composer Studio
    CCS 7.3.0

Although some projects for older IDE versions may be included, these projects
and the corresponding IDEs have not been tested with this SDK release.

Visual Studio 2017 Notes
* The Windows Universal CRT SDK and MFC packages must be installed for Visual
  Studio 2017 compatibility
* 32-bit architecture configurations have been removed from the Visual Studio 2017
  projects in this SDK
* The example plug-in projects have not been updated for or tested with Visual
  Studio 2017


---
Documentation Notes

The SDK documentation is also provided in PDF form. The PDF documentation is not
recommended for browsing but can be very useful for text-based searches.

If you wish to recompile the code-generated documentation:
  1) Download and install the latest version of Doxygen from www.doxygen.org
  2) (Optional for dot) Download and install the latest Graphviz tools from
     www.graphviz.org
  3) (Optional for MSC) Download and install the latest version of the mscgen tool
     from http://www.mcternan.me.uk/mscgen/
  4) Run Doxygen from the AAX_SDK/Documentation/Doxygen directory using the
     included AAX_SDK_Doxyfile config file. On Mac, simply run the
     generate_aax_sdk_doxygen.sh script to re-compile and format the documentation.
     This script requires that the doxygen command-line program is included in your
     PATH variable or within /Applications/Doxygen.app
