#include"Recources/File_reader.hpp"

#define FREAD(x,size)   read((char*)&x, size)
#define FWRITE(x, size) write((char*)&x, size)

//.tung
bool tung_buffer_verify(char buffer[], size_t size, std::string str) {
	for (int i = 0; i < size; i++) {
		if (buffer[i] != str[i]) {
			return 1;
		}
	}
	return 0;
}

tungfile readtung(std::wstring ipath) {

	std::ifstream filestream;
	filestream.open(ipath, std::ios::binary);

	tungfile file;

	//can we open it?
	if (!filestream.is_open()) {
		file.__Status__ = CFR_ERR_CANT_OPEN;
		filestream.close();
		return file;
	}
	
	char buffer[16];
	
	//header
	filestream.FREAD(buffer, 16);
	if (tung_buffer_verify(buffer,16,"Logic World save")) {
		file.__Status__ = CFR_ERR_INVALID_HEADER;
		filestream.close();
		return file;
	}

	//save format version
	filestream.FREAD(buffer, 1);
	if (buffer[0] != 4) {
		file.__Status__ = CFR_ERR_INVALID_VERSION;
		filestream.close();
		return file;
	}
	
	//game version
	filestream.FREAD(buffer, 4);
	memcpy(&file.GameVersion[0], buffer, 4);
	filestream.FREAD(buffer, 4);
	memcpy(&file.GameVersion[1], buffer, 4);
	filestream.FREAD(buffer, 4);
	memcpy(&file.GameVersion[2], buffer, 4);
	filestream.FREAD(buffer, 4);
	memcpy(&file.GameVersion[3], buffer, 4);

	//component and wire count
	filestream.FREAD(buffer, 4);
	memcpy(&file.count_components, buffer, 4);
	filestream.FREAD(buffer, 4);
	memcpy(&file.count_wires, buffer, 4);

	//how many ids are in the file
	filestream.FREAD(buffer, 4);
	memcpy(&file.componentIDs, buffer, 4);

	file.ID_Map = std::make_unique<componentid[]>(file.componentIDs);
	for (uint32_t i = 0; i < file.componentIDs; i++) {
		
		filestream.FREAD(buffer, 2);
		memcpy(&file.ID_Map[i].id, buffer, 2);
		filestream.FREAD(buffer, 4);
		memcpy(&file.ID_Map[i].length, buffer, 4);
		
		char* B_two = new char[(file.ID_Map[i].length) + 1];
		filestream.read(B_two, file.ID_Map[i].length);
		
		file.ID_Map[i].text_id = std::make_unique<char[]>(file.ID_Map[i].length);
		for (uint32_t j = 0; j < file.ID_Map[i].length; j++) {
			file.ID_Map[i].text_id[j] = B_two[j];
		}
		
		delete[] B_two;
	}


	file.components = std::make_unique<component[]>(file.count_components);
	for (int i = 0; i < file.count_components; i++) {

		filestream.FREAD(buffer,4);
		memcpy(&file.components[i].address, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].parent_address, buffer, 4);
		filestream.FREAD(buffer, 2);
		memcpy(&file.components[i].id, buffer, 2);

		//location
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].loc_x, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].loc_y, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].loc_z, buffer, 4);

		//rotation
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot_r, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot_i, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot_j, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot_k, buffer, 4);
		
		//input array
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].inputs, buffer, 4);
		
		file.components[i].in_arr = std::make_unique<uint8_t[]>(file.components[i].inputs);
		char* B_two = new char[(file.components[i].inputs) + 1];
		filestream.read(B_two, file.components[i].inputs);
		for (int j = 0; j < file.components[i].inputs; j++) {
			file.components[i].in_arr[j] = B_two[j];
		}
		delete[] B_two;

		//output array
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].outputs, buffer, 4);

		file.components[i].out_arr = std::make_unique<uint8_t[]>(file.components[i].outputs);
		char* B_three = new char[(file.components[i].outputs) + 1];
		filestream.read(B_three, file.components[i].outputs);

		for (int j = 0; j < file.components[i].outputs; j++) {
			file.components[i].out_arr[j] = B_three[j];
		}

		delete[] B_three;

		//byte array
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].byte_arr_size, buffer, 4);

		//if we need one
		if (file.components[i].byte_arr_size >= 1) {
			file.components[i].byte_arr = std::make_unique<uint8_t[]>(file.components[i].byte_arr_size);
			char* B_four = new char[(file.components[i].byte_arr_size) + 1];
			filestream.read(B_four, file.components[i].byte_arr_size);
			for (int j = 0; j < file.components[i].byte_arr_size; j++) {
				file.components[i].byte_arr[j] = B_four[j];
			}
			delete[] B_four;
		}
	}

	//wires
	file.wires = std::make_unique<wire[]>(file.count_wires);
	for (int i = 0; i < file.count_wires; i++) {
		filestream.FREAD(buffer, 16);
		memcpy(&file.wires[i], buffer, 16);
	}

	//footer
	filestream.FREAD(buffer, 16);
	if (tung_buffer_verify(buffer, 16, "redstone sux lol")) {
		file.__Status__ = CFR_ERR_INVALID_FOOTER;
		filestream.close();
		return file;
	}

	filestream.close();
	file.__Status__ = CFR_SUCCESS;
	return file;
}

void writetung(tungfile& file, std::wstring opath) {
	
	std::ofstream filestream;
	filestream.open(opath, std::ios::binary);

	char buffer[16];

	//idk, it works
	buffer[0]  = 'L';
	buffer[1]  = 'o';
	buffer[2]  = 'g';
	buffer[3]  = 'i';
	buffer[4]  = 'c';
	buffer[5]  = ' ';
	buffer[6]  = 'W';
	buffer[7]  = 'o';
	buffer[8]  = 'r';
	buffer[9]  = 'l';
	buffer[10] = 'd';
	buffer[11] = ' ';
	buffer[12] = 's';
	buffer[13] = 'a';
	buffer[14] = 'v';
	buffer[15] = 'e';

	//header
	filestream.FWRITE(buffer, 16);
	
	//save format version
	buffer[0] = 4;
	filestream.FWRITE(buffer, 1);

	//game version
	filestream.FWRITE(file.GameVersion, 16);

	//component counts
	filestream.FWRITE(file.count_components, 4);
	filestream.FWRITE(file.count_wires, 4);
	filestream.FWRITE(file.componentIDs, 4);
	
	//Component ID map
	for (int i = 0; i < file.componentIDs; i++) {
		filestream.FWRITE(file.ID_Map[i].id, 2);
		filestream.FWRITE(file.ID_Map[i].length, 4);
		for (int j = 0; j < file.ID_Map[i].length; j++) {
			filestream.FWRITE(file.ID_Map[i].text_id[j], 1);
		}
	}

	//components
	for (int i = 0; i < file.count_components; i++) {
		filestream.FWRITE(file.components[i].address, 4);
		filestream.FWRITE(file.components[i].parent_address, 4);
		filestream.FWRITE(file.components[i].id, 2);

		filestream.FWRITE(file.components[i].loc_x, 4);
		filestream.FWRITE(file.components[i].loc_y, 4);
		filestream.FWRITE(file.components[i].loc_z, 4);

		filestream.FWRITE(file.components[i].rot_r, 4);
		filestream.FWRITE(file.components[i].rot_i, 4);
		filestream.FWRITE(file.components[i].rot_j, 4);
		filestream.FWRITE(file.components[i].rot_k, 4);

		filestream.FWRITE(file.components[i].inputs, 4);
		if (file.components[i].inputs >= 1) {
			for (int j = 0; j < file.components[i].inputs; j++) {
				filestream.FWRITE(file.components[i].in_arr[j], 1);
			}
		}

		filestream.FWRITE(file.components[i].outputs, 4);
		if (file.components[i].outputs >= 1) {
			for (int j = 0; j < file.components[i].outputs; j++) {
				filestream.FWRITE(file.components[i].out_arr[j], 1);
			}
		}

		filestream.FWRITE(file.components[i].byte_arr_size, 4);
		if (file.components[i].byte_arr_size >= 1) {
			for (int j = 0; j < file.components[i].byte_arr_size; j++) {
				filestream.FWRITE(file.components[i].byte_arr[j], 1);
			}
		}
	}

	//wires
	for (int i = 0; i < file.count_wires; i++) {
		filestream.FWRITE(file.wires[i], sizeof(wire));
	}

	//idk, it works
	buffer[0]  = 'r';
	buffer[1]  = 'e';
	buffer[2]  = 'd';
	buffer[3]  = 's';
	buffer[4]  = 't';
	buffer[5]  = 'o';
	buffer[6]  = 'n';
	buffer[7]  = 'e';
	buffer[8]  = ' ';
	buffer[9]  = 's';
	buffer[10] = 'u';
	buffer[11] = 'x';
	buffer[12] = ' ';
	buffer[13] = 'l';
	buffer[14] = 'o';
	buffer[15] = 'l';

	filestream.FWRITE(buffer, 16);

	filestream.close();
};