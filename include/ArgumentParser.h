#pragma once
#include <string>
#include <vector>

class ArgumentParser
{
public:
	void parse(int argc, char** argv);
	bool exists(const std::string& arg);
	std::string find(const std::string& arg);
	bool find(const std::string& arg, std::string& value);
	void log_all();
private:
	std::vector<std::string> arguments;
};

