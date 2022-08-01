#include <iostream>

#include "AutonReader.h"
#include "RecordingFile.h"

int main() {

	AutonReader auton("res/Auton.xml");
	RecordingFile record("C:\\OpenSpace\\OpenSpace-0.18.0\\user\\recordings\\", auton.getAutonName());

	record.setTime(auton.getAutonTime());

	printf("Smoothing...\n");
	//auton.reducePoints();
	//auton.interpolatePoints();
	//auton.interpolatePoints();

	printf("Calculating rotations ...\n");
	auton.computeView();

	printf("Recording steps... \n");
	for (int i = 0; i < auton.getNumSteps(); i++) {
		record.add(auton.getStep(i));
	}

	record.close();

	printf("Done \n");

}
