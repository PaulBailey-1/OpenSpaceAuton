#include "RecordingFile.h"

RecordingFile::RecordingFile(std::string outputPath, std::string name, double speed) {

	printf("Opening recording... \n");

	_time = 0.0;
	_speed = speed;

	_file.open((outputPath + name + ".osrectxt"));

	_file << "OpenSpace_record/playback01.00A\n";
	_file << "script 0.0 0 0.0  1 openspace.time.setPause(true)\n";
	_file << "script 0.0 0 0.0  1 openspace.time.setDeltaTime(1)\n";

}

void RecordingFile::add(AutonReader::Step step) {
	char line[200];
	std::snprintf(line, 200, "camera %.7f %.7f 0.0 %.7f %.7f %.7f %.7f %.7f %.7f %.7f 2.0e-01 F Earth\n", _time, _time, step.pos.x, step.pos.y, step.pos.z, step.rot.x, step.rot.y, step.rot.z, step.rot.w);
	_file << line;
	_time += _speed;
}

void RecordingFile::close() {
	_file.close();
}
