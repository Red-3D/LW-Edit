#include"Recources/File_reader.hpp"

#define FREAD(x,size)   read((char*)&x, size)
#define FWRITE(x, size) write((char*)&x, size)

//tiny funcs
bool tung_buffer_verify(char buffer[], size_t size, std::string str) {
	for (int i = 0; i < size; i++) {
		if (buffer[i] != str[i]) {
			return 1;
		}
	}
	return 0;
}
void remove_comments(std::string& string) {
	size_t pos = string.find_first_of('#');
	while (pos != std::string::npos) {
		if (string[pos - 1] != '\\') {
			string.erase(pos, string.length());
			goto escape;
		}
		pos = string.find_first_of('#', pos + 1);
	}
escape:
	//remove escape for # (well add them when writing to file again)
	string.erase(std::remove(string.begin(), string.end(), '\\'), string.end());
	return;
}
bool get_val(std::string& string, std::string var_name) {
	size_t pos = 0;
	size_t pos2 = 0;
	//match variable name
	while (pos2 < var_name.length()) {
		if (string[pos] != ' ') {
			if (string[pos] != var_name[pos2]) {
				return 0;
			}
			pos2++;
		}
		pos++;
	}
	//try to get to the right position
	while (pos < string.length()) {
			pos++;
			while (pos < string.length()) {
				//find start of value
				if ((string[pos] != ' ') && (string[pos] !=':') && (string[pos] != '-')) {
					string.erase(0, pos);
					return 1;
				}
				pos++;
			}
			// if we got here the variable must be empty
			string = "";
		}
	return 1;
}
bool read_bool(std::string& string) {
	if (string == "true") return 1;
	return 0;
}

tungfile readtung(std::wstring ipath) {

	std::ifstream filestream;
	tungfile file;
	filestream.open(ipath, std::ios::binary);

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
		memcpy(&file.components[i].loc.x, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].loc.y, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].loc.z, buffer, 4);

		//rotation
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot.w, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot.x, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot.y, buffer, 4);
		filestream.FREAD(buffer, 4);
		memcpy(&file.components[i].rot.z, buffer, 4);
		
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
	if (!filestream.is_open()) {
		std::wcout << L"\n\nERROR: could not open path: " << opath;
		filestream.close();
		return;
	}

	char buffer[16];

	//header
	filestream.FWRITE("Logic World save", 16);
	
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
	for (uint32_t i = 0; i < file.componentIDs; i++) {
		filestream.FWRITE(file.ID_Map[i].id, 2);
		filestream.FWRITE(file.ID_Map[i].length, 4);
		for (uint32_t j = 0; j < file.ID_Map[i].length; j++) {
			filestream.FWRITE(file.ID_Map[i].text_id[j], 1);
		}
	}

	//components
	for (int i = 0; i < file.count_components; i++) {
		filestream.FWRITE(file.components[i].address, 4);
		filestream.FWRITE(file.components[i].parent_address, 4);
		filestream.FWRITE(file.components[i].id, 2);

		filestream.FWRITE(file.components[i].loc.x, 4);
		filestream.FWRITE(file.components[i].loc.y, 4);
		filestream.FWRITE(file.components[i].loc.z, 4);

		filestream.FWRITE(file.components[i].rot.w, 4);
		filestream.FWRITE(file.components[i].rot.x, 4);
		filestream.FWRITE(file.components[i].rot.y, 4);
		filestream.FWRITE(file.components[i].rot.z, 4);

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

	//footer
	filestream.FWRITE("redstone sux lol", 16);

	filestream.close();
};

//world (.succ)
lw_world readworld(std::wstring ipath) {

	std::wstring path;
	std::string line;
	lw_world world;
	bool found = false;

	//meta.succ
	path = ipath + L"\\meta.succ";
	if(std::filesystem::exists(path)) {
		
		std::ifstream file(path);
		//Title
		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "Title")) {
				world.Title = line;
				found = true;
			}
		}
		
		found = false;
		file.seekg(std::ios::beg);

		//Description
		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "Description")) {
				world.Description = line;
				found = true;
			}
		}

		found = false;
		file.seekg(std::ios::beg);

		//Tags position
		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "Tags")) {
				found = true;
			}
		}

		found = false;

		//actual tags
		while ((!found) && (!file.eof())) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "-")) {
				world.Tags += line + "\n";
			}
			else {
				found = true;
			}

		}

		file.close();
	}

	//worldinfo.succ
	path = ipath + L"\\worldinfo.succ";
	if (std::filesystem::exists(path)) {
		std::ifstream file(path);

		found = false;

		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "WorldTypeID")) {
				world.type = line;
				found = true;
			}
		}

		found = false;
		file.seekg(std::ios::beg);

		//worldspawn : x
		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "x")) {
				world.spawn[0] = std::stof(line);
				found = true;
			}
		}

		found = false;
		file.seekg(std::ios::beg);

		//worldspawn : y
		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "y")) {
				world.spawn[1] = std::stof(line);
				found = true;
			}
		}

		found = false;
		file.seekg(std::ios::beg);

		//worldspawn : z
		while (!found && !file.eof()) {
			getline(file, line);
			remove_comments(line);
			if (get_val(line, "z")) {
				world.spawn[2] = std::stof(line);
				found = true;
			}
		}

		file.close();
	}
	
	path = ipath + L"\\players\\";
	
	//players
	if (std::filesystem::exists(path)) {

		std::vector<std::wstring> paths;
		std::vector<std::wstring> player_ids;

		//iterate through all files in the player folder
		for (auto& p : std::filesystem::directory_iterator(path)) {
			paths.push_back(p.path());
			player_ids.push_back(p.path().filename());
		}

		//initialize player variables
		world.players = std::make_unique<player[]>(paths.size());
		if (paths.size() >= 1) {
			world.player_amount = paths.size();
		}

		//for every player
		for (int i = 0; i < paths.size(); i++) {

			//get the id out of the filename
			player_ids[i].erase(0, player_ids[i].find('{') + 1);
			player_ids[i].erase(player_ids[i].find('}'), player_ids[i].length());
			world.players[i].id = player_ids[i];

			std::ifstream file(paths[i]);
			
			found = false;

			//pos : x
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "x")) {
					world.players[i].position.x = std::stof(line);
					found = true;
				}
			}

			found = false;
			file.seekg(std::ios::beg);

			//pos : y
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "y")) {
					world.players[i].position.y = std::stof(line);
					found = true;
				}
			}

			found = false;
			file.seekg(std::ios::beg);

			//pos : z
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "z")) {
					world.players[i].position.z = std::stof(line);
					found = true;
				}
			}

			found = false;
			file.seekg(std::ios::beg);

			//rot : h
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "HeadHorizontalRotation")) {
					world.players[i].rotation.z = std::stof(line);
					found = true;
				}
			}

			found = false;
			file.seekg(std::ios::beg);

			//rot: v
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "HeadVerticalRotation")) {
					world.players[i].rotation.x = std::stof(line);
					found = true;
				}
			}

			found = false;
			file.seekg(std::ios::beg);

			//scale
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "Scale")) {
					world.players[i].scale = std::stof(line);
					found = true;
				}
			}

			found = false;
			file.seekg(std::ios::beg);

			//is flying
			while (!found && !file.eof()) {
				getline(file, line);
				remove_comments(line);
				if (get_val(line, "Flying")) {
					world.players[i].flying = read_bool(line);
					found = true;
				}
			}
			file.close();
		}
	}
	return world;
}

void writeworld(lw_world& fike, std::wstring opath) {

	return;
}