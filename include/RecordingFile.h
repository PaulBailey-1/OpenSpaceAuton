#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "AutonReader.h"

class RecordingFile {
public:

	RecordingFile(std::string outputPath, std::string name);

	void add(AutonReader::Step step);
	void close();

private:

	std::ofstream _file;
	double _time;
	double _speed;

};