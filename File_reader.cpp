#include"Recources/File_reader.hpp"

//.tung
#pragma pack(push,1)
struct tungfile
{	
	char Header[16];

	uint8_t SaveFormatVersion;
	int32_t GameVersion[4];
	uint32_t components;
	uint32_t wires;
};
#pragma pack(pop)

void readtung(std::wstring ipath) {

	std::ifstream filestream;
	filestream.open(ipath, std::ios::in);

	if (!filestream.is_open()) {
		MessageBox(NULL, L"could not open file", NULL, MB_ICONWARNING);
		filestream.close();
		return;
	}
	
	tungfile file;
	filestream.read((char*)&file, sizeof(file));

	std::wcout << "Header: " << file.Header << "\nSave Format Version:" << (unsigned int)file.SaveFormatVersion << "\nGame Version: " << file.GameVersion[0] << ' ' << file.GameVersion[1] << ' ' << file.GameVersion[2] << ' ' << file.GameVersion[3] << "\ncomponents: " << file.components << "\nwires:" << file.wires << '\n';

	filestream.close();
}