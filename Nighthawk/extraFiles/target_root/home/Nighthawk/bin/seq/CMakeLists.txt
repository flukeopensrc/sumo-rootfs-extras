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

file(GLOB ANALOG_SEQ_FILES
    "*.json"
)

add_custom_target(seq SOURCES ${ANALOG_SEQ_FILES})

find_program(JSONLINT NAMES jsonlint)

if (EXISTS ${JSONLINT})
    add_custom_target(seqval ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json
    )

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json
        COMMENT "Validating sequence JSON files"
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a4.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a5.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a6.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a7.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a8.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a10.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/a14.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/common.json -q
        COMMAND jsonlint ${CMAKE_CURRENT_SOURCE_DIR}/cadmus.json -q
    )
endif()

