#!/bin/bash
#
HELP_TEXT="AAX SDK documentation generator script\n
\n
Copyright 2014-2019 by Avid Technology, Inc.\n
\n
Requirements:\n
  doxygen must be installed in PATH or at
    /Applications/Doxygen.app/Contents/Resources/doxygen\n
  For PDF generation, pdflatex must be available in PATH\n
\n
Arguments:\n
  --help or -h: Help\n
  --pdf: Generate PDF\n
"

DOXYGEN_PROGRAM_NAME="doxygen"
DOXYGEN_PROGRAM_FILE="/Applications/Doxygen.app/Contents/Resources/$DOXYGEN_PROGRAM_NAME"


#
# Parse arguments
#

HELP_OPT_SHORT="-h"
HELP_OPT_LONG="--help"
PDF_OPT="--pdf"

GENERATE_PDF=0
if [ $# -gt 0 ]; then
  if [ "$1" == "$HELP_OPT_SHORT" -o "$1" == "$HELP_OPT_LONG" ]; then
    echo -e $HELP_TEXT
    exit 1
  elif [ "$1" == "$PDF_OPT" ]; then
    echo --pdf: pdf will be generated
    let GENERATE_PDF=1
  fi
fi


#
# Initialize the shell
echo initializing shell...
#

# Change to the shell's directory
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
#DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd && echo x)"
#DIR="${DIR%x}"
SUBDIR="/."
DOCDIR="$DIR/.."
cd "$DIR$SUBDIR"


#
# Run Doxygen
echo building doxygen...
#

did_find_doxygen_program=true

if type -P "$DOXYGEN_PROGRAM_NAME" > /dev/null; then
  DOXYGEN_PROGRAM_COMMAND="$(which $DOXYGEN_PROGRAM_NAME)"
elif [[ -f $DOXYGEN_PROGRAM_FILE ]]; then
  DOXYGEN_PROGRAM_COMMAND="$DOXYGEN_PROGRAM_FILE"
else
  did_find_doxygen_program=false
fi

if $did_find_doxygen_program; then
  # Remove any previous Doxygen output
  echo   cleaning up previous output
  if [ -d "output" ]; then
    rm -r output
  fi

  # Run Doxygen
  # We use parentheses to run as a subprocess; this gets our desired output
  # redirection to work from within the shell script execution regardless of
  # how redirection was set up when invoking the script itself.
  DOXYGEN_FULL_COMMAND="( $DOXYGEN_PROGRAM_COMMAND AAX_SDK_Doxyfile ) > doxygen_log.txt"
  echo   running doxygen with: $DOXYGEN_FULL_COMMAND
  eval $DOXYGEN_FULL_COMMAND
else
  echo   skipping - doxygen cannot be found in PATH or at $DOXYGEN_PROGRAM_FILE
  exit 1
fi

#
# Fix up the HTML output
echo   fixing html
#

# Doxygen does not copy over images that are included using the <IMG> tag, so we must manually copy the contents of the /img directory to output
if [ -d "output/html" ]; then
  if [ -d "img" ]; then
    cp -r img/ output/html/
  fi
fi

# Put dot images into 'centered' class
perl -e 's#\<img(.*?)src\=\"dot_#\<img class="centered"\1src\=\"dot_#g;' -pi $(find ${DIR}/output/html/*.html -type f)

if [ -f ${DIR}/output/html/index.html ]; then
  echo   html output generated at: ${DIR}/output/html/index.html
else
  echo   no html output found
fi

#
# Generate the PDF output from LaTeX
#

if [ $GENERATE_PDF -eq 1 ]; then
  # See http://stackoverflow.com/questions/592620/check-if-a-program-exists-from-a-bash-script/677212#677212
  if command -v pdflatex >/dev/null 2>&1; then
    echo generating pdf...
    cd "$DIR/output/latex"

    # set non-interactive mode
    sed -i "" 's/pdflatex refman/pdflatex -interaction nonstopmode refman/g' Makefile

    # build the pdf output
    make pdf >> doxygen_latex_to_pdf_log.txt

    # move the pdf to the main documentation folder
    mv refman.pdf ${DOCDIR}/doxygen.pdf
    echo   pdf output copied to: ${DOCDIR}/doxygen.pdf

    cd "$DIR$SUBDIR"
  else
    echo pdf generation skipped due to missing pdflatex
  fi
else
  echo pdf generation skipped due to missing --pdf argument
fi
