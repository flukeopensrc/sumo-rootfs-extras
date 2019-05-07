#!/usr/bin/awk -f

# Initial values for HPS UART lines in file
BEGIN {
	uart_begin = 0;
	uart_end = 0;
	sopc_begin = 0;
}

# Read the entire .dts file into an array.
# Note locations of start and end of HPS UARTs, and beginning of
# SOPC component.
{
	dts[NR] = $0;
        if ($0 ~ /hps_0_bridges:/) { sopc_begin = NR; };
        if ($0 ~ /hps_0_uart0:/) { uart_begin = NR; };
        if ($0 ~ /\(hps_0_uart1\)/) { uart_end = NR; };
}

# Spit out the array contents, re-locating the UARTs before the HPS bridges.
END {
	# Print from the beginning of the DTS to the HPS bridge.
	for (i = 1; i < (sopc_begin - 1); i++) {
		print dts[i];
        };

	# Print the HPS UARTs.
	for (i = (uart_begin - 1); i <= uart_end; i++) {
		print dts[i];
	};

	# Print the rest of the DTS up to the first HPS UART
	for (i = (sopc_begin - 1); i < (uart_begin - 1); i++) {
		print dts[i];
	};

	# Skip over the HPS UARTs and print to the end of the DTS
	for (i = (uart_end + 1); i <= NR; i++) {
		print dts[i];
	};
}
