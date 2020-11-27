#!/bin/bash

export TELEGRAF_PARSER_HOME=${HOME}/repo/telegraf_kdb_handler
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TELEGRAF_PARSER_HOME}/q/
