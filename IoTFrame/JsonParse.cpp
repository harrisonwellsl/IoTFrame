#include "JsonParse.h"

JsonParse::JsonParse(const char* json_str) {
	if (json_str == NULL || strcmp(json_str, "") == 0) {
		fprintf(stderr, "Give a invalid argument.");
		// �����������ΪNULLָ����߿��ַ���
		exit(EXIT_FAILURE);
	}
	this->document.SetObject();
	this->document.Parse(json_str);
}

JsonParse::~JsonParse() {
}

void JsonParse::CheckArguments(const char* member_name) const {
	if (member_name == NULL || strcmp(member_name, "") == 0) {
		// �����������ΪNULLָ����߿��ַ���
		fprintf(stderr, "Give a invalid argument.");
		exit(EXIT_FAILURE);
	}
}

int JsonParse::GetIntMember(const char* member_name) const {
	// ��鴫�����
	this->CheckArguments(member_name);

	if (!(this->document[member_name].IsInt())) {
		// ��Ա������������
		fprintf(stderr, "Give a member who's value isn't a integer.");
		exit(EXIT_FAILURE);
	}

	return this->document[member_name].GetInt();
}

std::string JsonParse::GetStrMember(const char* member_name) const {
	// ��鴫�����
	this->CheckArguments(member_name);

	if (!(this->document[member_name].IsString())) {
		// ��Ա������������
		fprintf(stderr, "Give a member who's value isn't a string.");
		exit(EXIT_FAILURE);
	}

	return std::string(this->document[member_name].GetString());
}
