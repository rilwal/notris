
// This file contains some utility functions I like to use for basic file parsing.
// They probably aren't very robust, and so shouldn't be used (or should be used with care)
// in real programs

#pragma once

#include <string>

void skip_whitespace(const char*& it);
void skip_whitespace_not_nl(const char*& it);
void skip_until_whitespace(const char*& it);
std::string get_next_token(const char*& it);
void skip_to_newline(const char*& it);
bool check_token(const char*& it, const std::string& str);
std::string consume_next_token(const char*& it);

std::string consume_string(const char*& it);


bool consume_token(const char*& it, const std::string& str);

float consume_float(const char*& it);
int consume_int(const char*& it);

bool maybe_consume_float(const char*& it, float& f);
bool maybe_consume_int(const char*& it, int& i);
