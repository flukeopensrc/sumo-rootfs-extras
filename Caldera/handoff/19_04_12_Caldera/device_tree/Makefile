
# Make file for Caldera device tree.

Caldera_soc.dtb:	Caldera_soc.dts
	dtc -O dtb Caldera_soc.dts -o Caldera_soc.dtb

#	sopc2dts --input ../../caldera_soc.sopcinfo \
#	--output caldera_soc.dtb \
#	--type dtb \
#	--board soc_system_board_info.xml \
#	--board hps_common_board_info.xml \
#	--bridge-removal all \
#	--clocks -v

Caldera_soc.dts:	../../Caldera_soc.sopcinfo caldera_board_info.xml \
		hps_common_board_info.xml
	sopc2dts --input ../../Caldera_soc.sopcinfo \
	--output Caldera_soc.dts \
	--type dts \
	--board caldera_board_info.xml \
	--board hps_common_board_info.xml \
	--bridge-removal all \
	--clocks -v
	./munge_serial.awk < Caldera_soc.dts > Caldera_soc.tmp
	mv Caldera_soc.tmp Caldera_soc.dts
