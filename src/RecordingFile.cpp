#include "RecordingFile.h"
#include <inttypes.h>

RecordingFile::RecordingFile(std::string outputPath, std::string name) {

	printf("Opening recording... \n");

	_time = 0.0;
	_simTime = 0;

	_file.open((outputPath + name + ".osrectxt"));

	_file << "OpenSpace_record/playback01.00A\n";
	_file << "script 0.0 0 0.0  1 openspace.time.setPause(false)\n";
	_file << "script 0.0 0 0.0  1 openspace.time.setDeltaTime(100)\n";

}

void RecordingFile::add(AutonReader::Step step) {
	char line[200];
	std::snprintf(line, 200, "camera %.7f %.7f %" PRId64 " %.7f %.7f %.7f %.7f %.7f %.7f %.7f 2.0e-01 F Earth\n", _time, _time, _simTime, step.pos.x, step.pos.y, step.pos.z, step.rot.x, step.rot.y, step.rot.z, step.rot.w);
	_file << line;

	_time += step.speed;
}

void RecordingFile::close() {
	_file.close();
}

void RecordingFile::setTime(int64_t time) {
	_simTime = time;
}