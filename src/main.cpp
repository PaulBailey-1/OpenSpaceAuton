#include <iostream>

#include "AutonReader.h"
#include "RecordingFile.h"

int main() {
	
	AutonReader auton("/res/Auton.xml");
	RecordingFile record("/", auton.getAutonName());

	AutonReader::Step step = auton.getNextStep();
	while(!step.end) {
		record << step;
	}

	record.close();

}
