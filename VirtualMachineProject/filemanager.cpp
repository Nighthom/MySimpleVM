#include "filemanager.h"

FileManager::FileManager(const char* path, const char* mode) {
	fopen_s(&m_fp, path, mode);
	if (!m_fp)
		return;

	if (mode == "r") {
		int size;
		int count;
 
		fseek(m_fp, 0, SEEK_END);					// ���� �����͸� ������ ������ �̵���Ŵ
		size = ftell(m_fp);						// ���� �������� ���� ��ġ�� ���� => ���� ������
 
		data = new char[size+1];				// ���� ũ�� + 1����Ʈ(���ڿ� �������� NULL) ��ŭ ���� �޸� �Ҵ�
		memset(data, 0, size + 1);			// ���� ũ�� +1����Ʈ��ŭ �޸� 0���� �ʱ�ȭ
 
		fseek(m_fp, 0, SEEK_SET);                // ���� �����͸� ������ ó������ �̵���Ŵ
		count = fread(data, size, 1, m_fp);    // ���ڵ忡�� ���� �о��
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
