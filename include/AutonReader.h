#pragma once
#include <string>
#include <iostream>
#include <vector>

#include "tinyxml2.h"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

struct Geo {

	Geo() {
		lat = 0.0;
		lon = 0.0;
		alt = 0.0;
	}

	Geo(double lat_, double lon_, double alt_) {
		lat = lat_;
		lon = lon_;
		alt = alt_;
	}

	double lat;
	double lon;
	double alt;
};

class AutonReader {
public:

	enum LookType {
		TARGET,
		FORWARD,
		BACKWARD,
		DOWN
	};

	struct Step {

		enum StepType {
			CAMERA,
			DELTATIME,
			SIMTIME
		};

		Step(Step::StepType type_) {
			type = type_;

			pos = glm::vec3();
			rot = glm::quat();
			target = glm::vec3();
			lookType = TARGET;
			speed = 0.0;
			time = 0.0;
			geo = Geo();

			interpolate = 0;
			altSpeed = true;

			deltaTime = 0;
			simTime = 0;
		}

		StepType type;

		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 target;
		LookType lookType;

		double speed;
		double time;

		Geo geo;

		int interpolate;
		int deltaTime;

		int64_t simTime;

		bool altSpeed;

	};

	AutonReader(std::string filepath);

	void interpolatePoints();
	int interpolateString(int startIndex, int endIndex, int loop);
	void computeView();
	void computeSpeeds();

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

	Geo parseCoordinates(const char* coords, bool flip = false, char delim = ' ');
	glm::vec3 geoToCoordinates(Geo point);
	Step getNextCamera(int current);

};