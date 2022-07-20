#include "RecordingFile.h"

RecordingFile::RecordingFile(std::string outputPath, std::string name) {
	_file.open((outputPath + name + ".TODO"));
}

void RecordingFile::add(AutonReader::Step step) {
	char line[10];
	std::sprintf(line, "camera %f %f %f %f %f %f %f\n", step.pos.x, step.pos.y, step.pos.z, step.rot.x, step.rot.y, step.rot.z, step.rot.w);
	_file << line;
}

void RecordingFile::close() {
	_file.close();
}
