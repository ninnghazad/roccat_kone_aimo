/*
 * Roccat Kone Aimo RGB led tool by ninnghazad (2020)
 *
 * g++ roccat_kone_aimo_rgb_tool.cpp -o roccat_kone_aimo_rgb_tool -I/usr/include -O3 -march=native
 *
 * ./roccat_kone_aimo_rgb_tool /dev/hidraw0 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00 FF 00 00
 *
 * Order of leds, one hex triplet per ( FF FF FF ):
 *  wheel
 *  left strip top
 *  left strip below top
 *  left strip above bottom
 *  left strip bottom
 *  right strip top
 *  right strip below top
 *  right strip above bottom
 *  right strip bottom
 *  left lower side
 *  right lower side
 *
 * based on:
 * Hidraw Userspace Example
 *
 * Copyright (c) 2010 Alan Ott <alan@signal11.us>
 * Copyright (c) 2010 Signal 11 Software
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using hidraw.
 */

/* Linux */
#include <linux/input.h>
#include <linux/hidraw.h>

#ifndef HIDIOCSFEATURE
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <array>
#include <sstream>
const char *bus_str(int bus);

void get_feature_report(int fd) {
	char buf[3];
	buf[0] = 0x4; /* Report Number */
	auto res = ioctl(fd, HIDIOCGFEATURE(3), buf);
	if (res < 0) {
		perror("HIDIOCGFEATURE");
	}
}

void set_feature_report(int fd,const uint8_t * msg,size_t len) {
	auto res = ioctl(fd, HIDIOCSFEATURE(len), msg);
	if (res < 0) {
		perror("HIDIOCSFEATURE");
	}
}

bool is_device_supported(const hidraw_devinfo &info)
{
	// Based on https://pcilookup.com/?ven=1e7d&dev=&action=submit
	// PCI devices above product ID 0x2e7d are keyboards
	// Tested: Kone Aimo, Kone Pure Ultra

	if (info.bustype != BUS_USB && info.bustype != BUS_BLUETOOTH)
		return false;

	if (info.vendor != 0x1e7d)
		return false; // Not ROCCAT

	return (info.product >= 0x2db4 && info.product <= 0x2e27); // known "Kone *" mice
}

int main(int argc, char **argv)
{
	if(argc < 35 && argc != 2 + 3) {
		std::cout << "Invalid syntax. Accepted parameters:"
			"\n\t/dev/hidrawX RR GG BB RR GG BB ... (total 11 RGB triplets)"
			"\n\t/dev/hidrawX RR GG BB (constant color)" << std::endl;
		return 1;
	}

	auto fd{open(argv[1], O_RDWR|O_NONBLOCK)};
	if (fd < 0) {
		perror("Unable to open hidraw device.");
		return 1;
	}

	hidraw_devinfo info;
	auto res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		perror("HIDIOCGRAWINFO");
	} else if (!is_device_supported(info)) {
		std::array<uint8_t,256> buf;

		std::cerr << "ERROR: This is not a Roccat Kone Aimo device." << std::endl;
		res = ioctl(fd, HIDIOCGRAWNAME(256), &buf[0]);
		if (res < 0)
			perror("HIDIOCGRAWNAME");
		else
			printf("Raw Name: %s\n", &buf[0]);

		res = ioctl(fd, HIDIOCGRAWPHYS(256), &buf[0]);
		if (res < 0)
			perror("HIDIOCGRAWPHYS");
		else
			printf("Raw Phys: %s\n", &buf[0]);
		printf("Raw Info:\n");
		printf("\tbustype: %d (%s)\n",
			info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}

	get_feature_report(fd);

	const std::array<uint8_t,6> msg_init{0x0e,0x06,0x01,0x01,0x00,0xff};

	set_feature_report(fd,&msg_init[0],6);
	set_feature_report(fd,&msg_init[0],6);

	const size_t MSG_LENGTH = 46;
	std::array<uint8_t,MSG_LENGTH> msg_colors{{0x0d,0x2e}};
	size_t msg_index = 2;

	// Input parsing
	if (argc == 2 + 3) {
		// Format: RRGGBB
		unsigned colors[] = {0, 0, 0, 0};
		sscanf(argv[2 + 0], "%x", &colors[0]); // R
		sscanf(argv[2 + 1], "%x", &colors[1]); // G
		sscanf(argv[2 + 2], "%x", &colors[2]); // B
		// colors[3] is unused / Alpha (0)

		for (; msg_index < MSG_LENGTH; ++msg_index) {
			// Loop through all 4 indices in the color array
			uint8_t c = colors[(msg_index - 2) % 4];
			msg_colors[msg_index] = c;
		}
	} else {
		// Format: ??
		size_t argc_index = 2;
		for (; msg_index < MSG_LENGTH; ++msg_index) {
			uint8_t c = 0;
			if ((msg_index - 1) % 4 > 0) {
				c = (uint8_t)std::stoul(std::string(argv[argc_index]), nullptr, 16);
				argc_index++;
			}
			msg_colors[msg_index] = c;
		}
	}
	set_feature_report(fd,&msg_colors[0], MSG_LENGTH);
	get_feature_report(fd);

	close(fd);
	return 0;
}

const char *
bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
	case BUS_HIL:
		return "HIL";
	case BUS_BLUETOOTH:
		return "Bluetooth";
	case BUS_VIRTUAL:
		return "Virtual";
	default:
		return "Other";
	}
}
