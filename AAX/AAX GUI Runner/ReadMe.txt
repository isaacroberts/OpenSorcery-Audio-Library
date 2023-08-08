AAX GUI Runner
v. 0.1
12/28/2017

Copyright 2015, 2017 Avid Technology, Inc.  All rights reserved.  This
application is part of the Avid Plug-In SDK and is Confidential Information of
Avid Technology, Inc.  Access to and use of this application is restricted to
Avid Third-Party Developers who are validly licensed under the Avid Third
Party Developer and Nondisclosure Agreement (the "Developer Agreement")
subject to the terms and conditions of the Developer Agreement.


================================================================================
AAX GUI Runner
================================================================================


--------------------------------------------------------------------------------
About
--------------------------------------------------------------------------------
The AAX GUI Runner is a simple AAX Host example application. The user may load
an AAX plug-in to see and interact with the plug-in GUI in a floating window.

This application is written in Objective C and uses the AAXHostShell framework
for AAX hosting.

This project is intended to be a simple example. All application source is
included. You are encouraged to use this example and the AAXHostShell framework
to create AAX hosting tools for your own internal plug-in development needs.

Please help us to improve this project! Notify Avid at devservices@avid.com with
any bug fixes or feature extensions which you create for this project. We will
incorporate your changes into a future release.

All materials in this package are provided as a part of the Avid Plug-In SDK and
are subject to the Developer Agreement.


--------------------------------------------------------------------------------
AAXHostShell
--------------------------------------------------------------------------------
AAXHostShell is a wrapper framework providing a pure C interface to Avid's AAX
plug-in hosting components.

The API for this framework is extended as needed. If you plan to make some
extension to the AAX GUI Runner project that requires a new AAXHostShell API
then please submit an API request to devservices@avid.com.

In addition to the AAXHostShell framework itself this package includes a set of
client utility files for C++ and Objective C clients. These utility classes
may be helpful if you plan to create your own AAX test host using AAXHostShell.

The AAXHostShell interface is documented using Doxygen markup. This package
does not include a build of the Doxygen documentation but if you have Doxygen
installed you can build the documentation files using the supplied Doxyfile_API
configuration file or the generate_api_doxygen.sh script.
