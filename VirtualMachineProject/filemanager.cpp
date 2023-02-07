#include "filemanager.h"

FileManager::FileManager(const char* path, const char* mode) {
	fopen_s(&m_fp, path, mode);
	if (!m_fp)
		return;

	if (mode == "r") {
		int size;
		int count;
 
		fseek(m_fp, 0, SEEK_END);					// 파일 포인터를 파일의 끝으로 이동시킴
		size = ftell(m_fp);						// 파일 포인터의 현재 위치를 얻음 => 파일 사이즈
 
		data = new char[size+1];				// 파일 크기 + 1바이트(문자열 마지막의 NULL) 만큼 동적 메모리 할당
		memset(data, 0, size + 1);			// 파일 크기 +1바이트만큼 메모리 0으로 초기화
 
		fseek(m_fp, 0, SEEK_SET);                // 파일 포인터를 파일의 처음으로 이동시킴
		count = fread(data, size, 1, m_fp);    // 옵코드에서 파일 읽어옴
		return;
	}
}

FileManager::~FileManager() {
	fclose(m_fp);
	delete data;
}

char* FileManager::readData() {
	return data;
}
