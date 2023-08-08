#!/bin/bash
#
#
# Copyright 2014 by Avid Technology, Inc.
#
#


# Change to the shell's directory
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
#DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd && echo x)"
#DIR="${DIR%x}"
SUBDIR="/API"
cd "$DIR$SUBDIR"

# Remove any previous Doxygen output
if [ -d "html" ]; then
	rm -r html
fi

# Run Doxygen
/Applications/Doxygen.app/Contents/Resources/doxygen DoxyFile_API >> doxygen_api_output.txt
