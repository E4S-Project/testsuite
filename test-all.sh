#!/bin/bash
settings_file="settings.sh"
for arg in "$@"; do
	if [[ $arg == --settings=* ]]; then
    		settings_file="${arg#--settings=}"
    		if [[ -f "$settings_file" ]]; then
			export TESTSUITE_SETTINGS_FILE=$settings_file
			echo "Sourced settings from $settings_file"
    		else
			echo "Settings file $settings_file does not exist."
      			exit 1
    		fi
  	fi
done
source $settings_file
source setup.sh
./test-all.py "$@"
