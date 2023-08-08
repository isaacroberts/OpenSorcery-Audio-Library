cmake_minimum_required(VERSION 3.13)

set(NAME "RectiFi")
set(TARGET_NAME "pi.aax.rectify")
set(RELATIVE_PATH_TO_PROTOOLS_ROOT_DIR "../../..")

if (NOT DEFINED PROTOOLS_ROOT_DIR)
	set(PROTOOLS_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/${RELATIVE_PATH_TO_PROTOOLS_ROOT_DIR}")
endif()

set(PLUGIN_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(SOURCE
	${PLUGIN_ROOT_DIR}/RectiFi_AlgProc.cpp
	${PLUGIN_ROOT_DIR}/RectiFi_Describe.cpp
	${PLUGIN_ROOT_DIR}/RectiFi_EffectParameters.cpp
)

set(PLUGIN_RESOURCES
	${PLUGIN_ROOT_DIR}/RectiFi.xml
)

set(INCLUDE_DIRS
	${PLUGIN_ROOT_DIR}/Source
)

# DO THE REST OF WORK
include(${PROTOOLS_ROOT_DIR}/AAX/PlugIns/Common/${CMAKE_SYSTEM_NAME}/plugin_common.cmake)
