#pragma once
#include <string>
#include <iostream>
#include <vector>

#include "tinyxml2.h"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

class AutonReader {
public:

	enum LookType {
		TARGET,
		FORWARD,
		BACKWARD
	};

	struct Step {
		Step(glm::vec3 _pos, glm::quat _rot) {
			pos = _pos;
			rot = _rot;
			speed = 0.0;
		}

		Step(glm::vec3 _pos, LookType _lookType, glm::vec3 _target, double _speed) {
			pos = _pos;
			lookType = _lookType;
			target = _target;
			speed = _speed;
		}

		Step(glm::vec3 _pos, LookType _lookType, double _speed) {
			pos = _pos;
			lookType = _lookType;
			speed = _speed;
		}

		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 target;
		LookType lookType;
		double speed;

		double lat;
		double lon;
		double alt;

	};

	AutonReader(std::string filepath);

	void interpolatePoints();
	void reducePoints();
	void computeView();
	void simulatePath(int startIndex, int endIndex);
	void equalizePath(int startIndex, int endIndex);

	int getNumSteps();
	Step getStep(int i);

	std::string getAutonName();
	int64_t getAutonTime();

private:

	tinyxml2::XMLDocument _file;
	tinyxml2::XMLElement* _auton;
	tinyxml2::XMLElement* _line;

	glm::vec3 _target;
	bool _lookForward;

	double _speed;

	std::vector<Step> _steps;

	glm::vec3 parseCoordinates(const char* coords, double altOffset = 0.0, bool flip = false, char delim = ' ');

};