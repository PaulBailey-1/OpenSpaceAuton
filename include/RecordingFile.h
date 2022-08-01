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

	void setTime(int64_t time);

private:

	std::ofstream _file;
	double _time;

	int64_t _simTime;

};