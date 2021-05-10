#!/bin/sh

# This script exists to choos the proper config file for the eGTouchD based 
# on which touchscreen is installed.  It is run from a udev rule, which passes
# in the vendor/product ids when the touchscreen is first detected.
# We copy the appropriate config file to /tmp and point a symbolic link at
# it since /tmp is a RAM based filesystem
# and we want to minimize writes to the flash that /etc is mounted from.

EGTOUCHD_CONF=eGTouchL.ini

if test -e "/etc/$EGTOUCHD_CONF"; then
	if ! test -h "/etc/$EGTOUCHD_CONF"; then
		echo "WARNING: /etc/$EGTOUCHD_CONF already exists and but is not a symbolic link."
	fi
else
	ln -s /tmp/$EGTOUCHD_CONF /etc/$EGTOUCHD_CONF
	sync
fi

cp /etc/$EGTOUCHD_CONF.vid_${ID_VENDOR_ID}_pid_${ID_MODEL_ID} /tmp/$EGTOUCHD_CONF
