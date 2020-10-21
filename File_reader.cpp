#include"Recources/File_reader.hpp"

#define FREAD(x,size) read((char*)&x,size)

//.tung

tungfile readtung(std::wstring ipath) {

	std::ifstream filestream;
	filestream.open(ipath, std::ios::binary);

	tungfile file;

	//can we open it?
	if (!filestream.is_open()) {
		MessageBox(NULL, L"could not open file", NULL, MB_ICONWARNING);
		filestream.close();
		return file;
	}
	
	char buffer[16];
	//header
	filestream.FREAD(buffer, 16);
	memcpy(&file.header, buffer, 16);
	//save format version
	filestream.FREAD(buffer, 1);
	memcpy(&file.SaveFormatVersion, buffer, 1);
	
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
	for (uint32_t i = 0; i < file.count_wires; i++) {
		filestream.FREAD(buffer, 16);
		memcpy(&file.wires[i], buffer, 16);
	}

	//footer
	filestream.FREAD(buffer, 16);
	memcpy(&file.footer, buffer, 16);

	filestream.close();
	file.__Status__ = 1;
	return file;
}

void writetung(tungfile file);