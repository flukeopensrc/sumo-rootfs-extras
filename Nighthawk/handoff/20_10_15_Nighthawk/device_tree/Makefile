
# Make file for Nighthawk device tree.

Nighthawk_soc.dtb:	Nighthawk_soc.dts
	dtc -O dtb Nighthawk_soc.dts -o Nighthawk_soc.dtb

#	sopc2dts --input ../../Nighthawk_soc.sopcinfo \
#	--output Nighthawk_soc.dtb \
#	--type dtb \
#	--board nighthawk_board_info.xml \
#	--board hps_common_board_info.xml \
#	--bridge-removal all \
#	--clocks -v

Nighthawk_soc.dts:	../../Nighthawk_soc.sopcinfo nighthawk_board_info.xml \
		hps_common_board_info.xml
	sopc2dts --input ../../Nighthawk_soc.sopcinfo \
	--output Nighthawk_soc.dts \
	--type dts \
	--board nighthawk_board_info.xml \
	--board hps_common_board_info.xml \
	--bridge-removal all \
	--clocks -v
	./munge_serial.awk < Nighthawk_soc.dts > Nighthawk_soc.tmp
	/bin/mv Nighthawk_soc.tmp Nighthawk_soc.dts
	sopc2dts --input ../../Nighthawk_soc.sopcinfo \
	--output Nighthawk_soc_raw.dts \
	--type dts \
	--board nighthawk_board_info_raw.xml \
	--bridge-removal all \
	--clocks -v
