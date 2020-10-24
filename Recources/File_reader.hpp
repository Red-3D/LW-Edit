#pragma once
#pragma warning(disable : 4996)

#ifndef _STRING_
#include<string>
#endif
#ifndef _IOSTREAM_
#include<iostream>
#endif
#ifndef _FSTREAM_
#include<fstream>
#endif
#include<filesystem>
#include<algorithm>

//Custom File Reader defs
#define CFR_SUCCESS             100
#define CFR_ERR_WHAT            1
#define CFR_ERR_CANT_OPEN       2
#define CFR_ERR_INVALID_HEADER  3
#define CFR_ERR_INVALID_VERSION 4
#define CFR_ERR_INVALID_FOOTER  5

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
	int8_t __Status__ = CFR_ERR_WHAT;
	int32_t GameVersion[4];
	int32_t count_components;
	int32_t count_wires;
	uint32_t componentIDs;
	std::unique_ptr<componentid[]> ID_Map;
	std::unique_ptr<component[]> components;
	std::unique_ptr<wire[]> wires;
};
#pragma pack(pop)

struct player {
	std::wstring id;
	float position[3] = {};
	float rotation[2] = {};
	float scale = 1;
	bool flying = false;
};
struct lw_world {
	uint8_t __Status__;
	std::string Title;
	std::string Description;
	std::string Tags;
	float spawn[3] = {0};
	uint32_t player_amount = 0;
	std::unique_ptr<player[]> players;

};

tungfile readtung(std::wstring path);
void writetung(tungfile& file, std::wstring opath);

lw_world readworld(std::wstring ipath);
