#pragma once
#include <string>
#include <iostream>
#include <vector>

#include "tinyxml2.h"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

class AutonReader {
public:

	struct Step {
		Step(glm::vec3 _pos, glm::quat _rot) {
			pos = _pos;
			rot = _rot;
		}

		Step(glm::vec3 _pos) {
			pos = _pos;
		}

		glm::vec3 pos;
		glm::quat rot;
	};

	AutonReader(std::string filepath);

	void interpolatePoints();
	void reducePoints();
	void computeView();

	int getNumSteps();
	Step getStep(int i);

	std::string getAutonName();
	double getAutonSpeed();

private:

	tinyxml2::XMLDocument _file;
	tinyxml2::XMLElement* _auton;
	tinyxml2::XMLElement* _line;

	glm::vec3 _target;
	bool _lookForward;

	std::vector<Step> _steps;

	glm::vec3 parseCoordinates(const char* coords, bool flip = false, char delim = ' ');

};