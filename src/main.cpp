#include <iostream>

#include "AutonReader.h"
#include "RecordingFile.h"

int main() {
	
	AutonReader auton("res/Auton.xml");
	RecordingFile record("C:\\OpenSpace\\OpenSpace-0.17.1\\user\\recordings\\", auton.getAutonName());

	AutonReader::Step step = auton.getNextStep();
	while(!step.end) {
		record.add(step);
		step = auton.getNextStep();
	}

	record.close();

}
