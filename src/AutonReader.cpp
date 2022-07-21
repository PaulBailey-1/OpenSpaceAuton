#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>

#include "AutonReader.h"

AutonReader::AutonReader(std::string filepath) {

	printf("Loading Auton... \n");

	if (_file.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
		printf("Error: Could not load file\n");
		throw;
	}

	_auton = _file.FirstChildElement("auton");
	if (_auton == nullptr) {
		printf("Error: No auton element found\n");
		throw;
	}

	_line = nullptr;
	_target = glm::vec3(0, 0, 0);

	while (true) {
		if (_line == nullptr) {
			_line = _auton->FirstChildElement();
		}
		else {
			_line = _line->NextSiblingElement();
		}

		if (_line == nullptr) {
			break;
		}

		if (std::strcmp(_line->Name(), "look") == 0) {

			_target = parseCoordinates(_line->GetText());

		}
		else if (std::strcmp(_line->Name(), "move") == 0) {

			glm::vec3 pos = parseCoordinates(_line->GetText());
			_steps.push_back(Step(pos));

		}
		else if (std::strcmp(_line->Name(), "coordinates") == 0) {
			const char* coords = _line->GetText();

			std::string str(coords);
			std::stringstream ss(str);
			std::string coord;

			while (ss >> coord) {

				glm::vec3 pos = parseCoordinates(coord.c_str(), true, ',');

				_steps.push_back(Step(pos));
			}
		}
		else {
			printf("Error: could not read xml\n");
		}
	}
}

void AutonReader::interpolatePoints() {
	for (int i = 0; i < _steps.size() - 1; i += 2) {

		glm::vec3 pos = (_steps[i].pos + _steps[i + 1].pos) / 2.0f;
		glm::quat rot = glm::quatLookAt(glm::normalize(_target - pos), glm::normalize(pos));

		Step step(pos, rot);

		_steps.insert(_steps.begin() + i + 1, step);
	}
}

void AutonReader::reducePoints() {
	for (int i = 0; i < _steps.size() - 2; i ++) {
		_steps.erase(_steps.begin() + i + 1);
	}
}

void AutonReader::computeView() {
	for (int i = 0; i < _steps.size() - 1; i++) {
		if (_target != glm::vec3(0,0,0)) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_target - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i + 1].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
	}

}

int AutonReader::getNumSteps() {
	return (int) (_steps.size());
}

AutonReader::Step AutonReader::getStep(int i) {
	return _steps[i];
}

std::string AutonReader::getAutonName() {
	return std::string(_auton->Attribute("name"));
}

double AutonReader::getAutonSpeed() {
	double speed;
	_auton->QueryDoubleAttribute("speed", &speed);
	return speed;
}

glm::vec3 AutonReader::parseCoordinates(const char* coords, bool flip, char delim) {

	std::string lats;
	std::string lons;
	std::string alts;

	std::string str(coords);
	std::stringstream ss(str);

	if (flip) {
		std::getline(ss, lons, delim);
		std::getline(ss, lats, delim);
	}
	else {
		std::getline(ss, lats, delim);
		std::getline(ss, lons, delim);
	}
	
	std::getline(ss, alts, delim);

	double lat = std::stod(lats);
	double lon = std::stod(lons);
	double alt = std::stod(alts);

	static const int earthRadius = 6378137;

	lat *= M_PI / 180;
	lon *= M_PI / 180;

	double distance = alt + earthRadius;
	return glm::vec3(cos(lon) * distance * cos(lat), sin(lon) * distance * cos(lat), distance * sin(lat));
}
