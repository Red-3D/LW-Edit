#pragma once
#ifndef _IOSTREAM_
#include<iostream>
#endif
#ifndef _FSTREAM_
#include<fstream>
#endif
#ifndef _WINDOWS_
#include<windows.h>
#endif

//shut, ima initialize it later
#pragma warning(disable : 26495)

#pragma pack(push,1)
struct componentid {
	uint16_t id;
	uint32_t length;
	std::unique_ptr<char[]> text_id;
};

struct component {
	int32_t address;
	int32_t parent_address;
	int16_t id;

	float loc_x;
	float loc_y;
	float loc_z;

	float rot_r;
	float rot_i;
	float rot_j;
	float rot_k;

	int32_t inputs;
	std::unique_ptr<uint8_t[]> in_arr;

	int32_t outputs;
	std::unique_ptr<uint8_t[]> out_arr;

	int32_t byte_arr_size;
	std::unique_ptr<uint8_t[]> byte_arr;
};

struct wire {
	int8_t inoutput_one;
	uint32_t address_one;
	int8_t index_one;

	int8_t inoutput_two;
	uint32_t address_two;
	int8_t index_two;

	float rotation;
};

struct tungfile {
	int8_t __Status__ = 0;					//will set to 1 if the file was read successfully
	char header[16];
	uint8_t SaveFormatVersion;
	int32_t GameVersion[4];
	int32_t count_components;
	int32_t count_wires;
	uint32_t componentIDs;
	std::unique_ptr<componentid[]> ID_Map;
	std::unique_ptr<component[]> components;
	std::unique_ptr<wire[]> wires;
	char footer[16];
};
#pragma pack(pop)

tungfile readtung(std::wstring path);