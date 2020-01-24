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

int main(int argc, char **argv)
{
	if(argc < 35) {
		std::cout << "specify first the path to hidraw* device, then all 11 rgb triplets as commandline parameter, as hex value." << std::endl;
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
	} else {
		if(info.vendor != 0x1e27 && info.product != 0x2e27) {
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
	}

	get_feature_report(fd);

	const std::array<uint8_t,6> msg_init{0x0e,0x06,0x01,0x01,0x00,0xff};

	set_feature_report(fd,&msg_init[0],6);
	set_feature_report(fd,&msg_init[0],6);

	std::array<uint8_t,46> msg_colors{{0x0d,0x2e}};
	for(int j = 2,k = 1;j < 46;++j) {
		if((j-1)%4 > 0) {
			++k;
			uint8_t c{static_cast<uint8_t>(std::stoul(std::string(argv[k]), nullptr, 16))};
			msg_colors[j] = c;
		} else msg_colors[j] = 0x00;
	}
	set_feature_report(fd,&msg_colors[0],46);
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
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}
