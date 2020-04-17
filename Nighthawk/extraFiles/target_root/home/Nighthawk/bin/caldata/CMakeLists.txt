#-------------------------------------------------
#
# Copyright (c) 2019 Fluke Corporation, Inc. All rights reserved.
# Use of the software source code and warranty disclaimers are
# identified in the Software Agreement associated herewith.
#
# Repository URL:    git@git.sesg.fluke.com:fcal/nighthawk
# Authored By:       B.J.Araujo
# Origin:            Nighthawk
#
# Project build file for the "Nighthawk" project.
#
#-------------------------------------------------

file(GLOB CAL_CALDATA_FILES
    "*.json"
)

add_custom_target(caldata SOURCES ${CAL_CALDATA_FILES})

find_program(JSONLINT NAMES jsonlint)

if (EXISTS ${JSONLINT})
    add_custom_target(caldataval ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json
    )

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json
        COMMENT "Validating caldata JSON files"
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/acidata.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/acirules.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/acvdata.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/acvrules.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/adjusted.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/dcidata.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/dcvdata.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/common.json -q
    )
endif()

