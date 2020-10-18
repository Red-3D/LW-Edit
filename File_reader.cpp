#include"Recources/File_reader.hpp"

#define fread(x,size) read((char*)&x,size)

//.tung
#pragma pack(push,1)
struct tungfile
{	
	char Header[16];

	uint8_t SaveFormatVersion;
	int32_t GameVersion[4];
	int32_t components;
	int32_t wires;
	uint32_t componentIDs;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct componentid
{
	uint16_t id;
	int32_t length;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct wire
{
	//first peg address
	bool inoutput_one;
	uint32_t address_one;
	int32_t index_one;

	//second peg address
	bool inoutput_two;
	uint32_t address_two;
	int32_t index_two;

	float rotation;
};
#pragma pack(pop)

void readtung(std::wstring ipath) {

	std::ifstream filestream;
	filestream.open(ipath, std::ios::binary);

	if (!filestream.is_open()) {
		MessageBox(NULL, L"could not open file", NULL, MB_ICONWARNING);
		filestream.close();
		return;
	}

	//Header | Save Format Version | Game Version | Number of components and wires | number of ids
	tungfile file;
	filestream.fread(file, sizeof(file));
	std::wcout << "Header: " << file.Header << "\nSave Format Version:" << (unsigned int)file.SaveFormatVersion << "\nGame Version: " << file.GameVersion[0] << ' ' << file.GameVersion[1] << ' ' << file.GameVersion[2] << ' ' << file.GameVersion[3] << "\ncomponents: " << file.components << "\nwires:" << file.wires << "\nComponent ID's: " << file.componentIDs << '\n';

	std::cout << "\n\nsize: " << sizeof(wire);

	system("pause");

	//Component ID map
	componentid id;
	char trash;
	std::string* names = new std::string[file.componentIDs];

	for (unsigned int i = 0; i < file.componentIDs; i++) {

		filestream.fread(id, sizeof(id));
		std::cout << "\n\nID: " << id.id << "\nlength: " << id.length << "\nUTF-8 ID: |";
//TODO optimize by reading the whole name at once
		for (int j = 0; j < id.length; j++) {
			filestream.fread(trash, 1);
			std::cout << trash;
			names[i] += trash;
		}
		std::cout << '|';
	}
	
	uint32_t address;
	uint32_t parent_address;
	int16_t type;
	float location[3];
	float rotation[4];

	int32_t inputs;
	int32_t outputs;
	int32_t length;

	std::cout << "pos: " << filestream.tellg();
	system("pause");

	for (int32_t i = 0; i < file.components; i++) {

		filestream.fread(address, 4);
		std::cout << "\n\nAdress: " << address;

		filestream.fread(parent_address, 4);
		std::cout << "\nParent Adress: " << parent_address;

		filestream.fread(type, 2);
		std::cout << "\ntype: " << (uint32_t)type << '|' << names[type];

		filestream.fread(location, 3 * sizeof(float));
		std::cout << "\nlocation: |" << location[0] << '|' << location[1] << '|' << location[2] << '|';

		filestream.fread(rotation, 4 * sizeof(float));
		std::cout << "\nrotation: |" << rotation[0] << '|' << rotation[1] << '|' << rotation[2] << '|' << rotation[3] << '|';

		filestream.fread(inputs, 4);
		std::cout << "\ninputs: " << inputs;

		bool* input_pegs = new bool[inputs];
		filestream.read((char*)input_pegs, inputs);

		filestream.fread(outputs, 4);
		std::cout << "\noutputs: " << outputs;

		bool* output_pegs = new bool[outputs];
		filestream.read((char*)output_pegs, outputs);

		filestream.fread(length, 4);
		std::cout << "\nbyte array length: " << length;

		if (length >= 1) {
			int8_t* bytearray = new int8_t[length];
			filestream.read((char*)bytearray, length);

			std::cout << "\nData: ";

			for (int32_t i = 0; i < length; i++) {
				std::cout << (int)bytearray[i] << ' ';
			}

			delete[] bytearray;
		}

		delete[] input_pegs;
		delete[] output_pegs;
	}

	volatile wire w_tmp;

	for (uint32_t i = 0; i < file.wires; i++) {
		
		/*if (5911 - filestream.tellg() <= 16) {
			goto end;
		}*/

		filestream.fread(w_tmp, sizeof(w_tmp));
		
		std::cout << "\n ID: " << i << " wire: " << w_tmp.address_one << ' ' << w_tmp.address_two << " pos: " << filestream.tellg();
	}

	end:

	char footer[16];
	filestream.fread(footer, 16);

	std::cout << "\n\nfooter: ";
	for (uint32_t i = 0; i < 16; i++) {
		std::cout << footer[i];
	}

	filestream.close();
}