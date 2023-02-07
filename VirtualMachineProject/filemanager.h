#pragma once
#include <stdio.h>
#include <memory.h>


class FileManager {
public:
	FileManager(const char*, const char*);
	~FileManager();

	char* readData();

private:
	FILE* m_fp;
	char* data;
};
