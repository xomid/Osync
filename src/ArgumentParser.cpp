#include "ArgumentParser.h"
#include <iostream>

void ArgumentParser::parse(int argc, char** argv) {
	// push all of the arguments into a vector in the incoming order 
	for (int i = 1; i < argc; ++i) 
		arguments.push_back(std::string(argv[i]));
}

bool ArgumentParser::exists(const std::string& arg) {
	return std::find(this->arguments.begin(), this->arguments.end(), arg)
		!= this->arguments.end();
}

std::string ArgumentParser::find(const std::string& option) {
	std::string value = "";
	find(option, value);
	return value;
}

bool ArgumentParser::find(const std::string& option, std::string& value) {
	// first look for the option
	auto itr = std::find(this->arguments.begin(), this->arguments.end(), option);

	// if found, look for the value of the option
	if (itr != this->arguments.end() && ++itr != this->arguments.end()) {
		// the next one is the value, unless the option name was the last element
		value = *itr;
		return true;
	}

	// nothing found, return an empty string
	return false;
}

void ArgumentParser::log_all() {
	for (auto v : arguments)
		std::cout << v << " ";
	std::cout << std::endl;
}