#include "parser.hpp"

void skip_whitespace(const char*& it) {
	while (it && *it && std::isspace(*it)) it++;
}


void skip_whitespace_not_nl(const char*& it) {
	while (it && *it && std::isspace(*it) && *it != '\n' && *it != '\r') it++;
}



void skip_until_whitespace(const char*& it) {
	while (it && *it && !std::isspace(*it)) it++;
}


std::string get_next_token(const char*& it) {
	const char* it2 = it;
	while (it2 && *it2 && !std::isspace(*it2) && *it2 != ';') it2++;
	return std::string(it, it2);
}


std::string consume_next_token(const char*& it) {
	const char* it2 = it;
	while (it && *it && !std::isspace(*it) && *it != ';') it++;
	return std::string(it2, it);
}


std::string consume_string(const char*& it) {
	consume_token(it, "\"");
	const char* start = it;
	while (it && *it && *it != '\"') it++;
	return std::string(start, it++);
}


// Skip until the next newline character
void skip_to_newline(const char*& it) {
	while (it && *it && *it != '\n') it++;
}


bool check_token(const char*& it, const std::string& str) {
	if (strncmp(it, str.c_str(), str.length()) == 0) {
		return true;
	}
	return false;
}



bool consume_token(const char*& it, const std::string& str) {
	if (check_token(it, str)) {
		it += str.length();
		return true;
	}
	return false;
}

float consume_float(const char*& it) {
	// Read a float from a string of characters
	char* end;
	float n = strtof(it, &end);
	it = end;
	return n;
}

int consume_int(const char*& it) {
	// Read a float from a string of characters
	char* end;
	int n = strtol(it, &end, 10);
	it = end;
	return n;
}


bool maybe_consume_float(const char*& it, float& f) {
	char* end;
	f = strtof(it, &end);

	if (end == it) {
		return false;
	}
	else {
		it = end;
		return true;
	}
}


bool maybe_consume_int(const char*& it, int& i) {
	char* end;
	i = strtol(it, &end, 10);

	if (end == it) {
		return false;
	}
	else {
		it = end;
		return true;
	}
}