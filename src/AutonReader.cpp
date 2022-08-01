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

	_auton->QueryDoubleAttribute("speed", &_speed);

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

		if (std::strcmp(_line->Name(), "lookTarget") == 0) {

			_target = parseCoordinates(_line->GetText());

		}
		else if (std::strcmp(_line->Name(), "move") == 0) {

			double speed = 0.0;
			_line->QueryDoubleAttribute("speed", &speed);

			if (speed == 0.0) {
				speed = _speed;
			}

			glm::vec3 pos = parseCoordinates(_line->GetText());
			_steps.push_back(Step(pos, LookType::TARGET, _target, speed));

		}
		else if (std::strcmp(_line->Name(), "coordinates") == 0) {

			LookType lookType;
			const char* look;
			_line->QueryStringAttribute("look", &look);

			if (strcmp(look, "forward") == 0) {
				lookType = FORWARD;
			}
			else if (strcmp(look, "backward") == 0) {
				lookType = BACKWARD;
			}
			else if (strcmp(look, "target") == 0) {
				lookType = TARGET;
			}

			double speed = 0.0;
			_line->QueryDoubleAttribute("speed", &speed);

			if (speed == 0.0) {
				speed = _speed;
			}

			double rise = 0.0;
			_line->QueryDoubleAttribute("rise", &rise);

			const char* coords = _line->GetText();

			std::string str(coords);
			std::stringstream ss(str);
			std::string coord;
			double riseOffset = 0.0;

			int startIndex = _steps.size();

			while (ss >> coord) {

				glm::vec3 pos = parseCoordinates(coord.c_str(), riseOffset, true, ',');

				_steps.push_back(Step(pos, lookType, pow(10, -0.000001 * riseOffset * log(speed))));

				riseOffset += rise;
			}

			//simulatePath(startIndex, _steps.size());
			//equalizePath(startIndex, _steps.size() - 1);
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
	for (int i = 0; i < _steps.size(); i++) {
		if (_steps[i].lookType == LookType::TARGET) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i].target - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else if (_steps[i].lookType == LookType::FORWARD && i < _steps.size() - 10) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[(int) (floor((i + 10) / 10.0) * 10)].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else if (_steps[i].lookType == LookType::BACKWARD && i >= 10) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i - 10].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else if (_steps[i].lookType == LookType::BACKWARD) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i - 1].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
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

int64_t AutonReader::getAutonTime() {
	int64_t time = 0;
	_auton->QueryInt64Attribute("time", &time);
	return time;
}

glm::vec3 AutonReader::parseCoordinates(const char* coords, double altOffset, bool flip, char delim) {

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
	alt += altOffset;

	static const int earthRadius = 6378137;

	lat *= M_PI / 180;
	lon *= M_PI / 180;

	double distance = alt + earthRadius;
	return glm::vec3(cos(lon) * distance * cos(lat), sin(lon) * distance * cos(lat), distance * sin(lat));
}

void AutonReader::simulatePath(int startIndex, int endIndex) {

	glm::vec3 posPerfect = _steps[startIndex].pos;
	glm::vec3 posActual = _steps[startIndex].pos;
	glm::vec3 velPerfect;
	glm::vec3 velActual;

	double timeStep = 0.1;

	for (int i = startIndex; i < endIndex; i++) {
		double steps = _steps[startIndex].speed / timeStep;
		int step = 0;
		while (step < steps) {
			glm::vec3 direction = _steps[startIndex + 1].pos - _steps[startIndex].pos;
			posPerfect += direction * (float) (1 / steps);
			step++;
		}
	}

}

void AutonReader::equalizePath(int startIndex, int endIndex) {

	double total = 0.0;
	for (int i = startIndex; i < endIndex; i++) {
		total += glm::length(_steps[i + 1].pos - _steps[i].pos);
	}
	double mean = total / (endIndex - startIndex);

	for (int i = startIndex; i < endIndex; i++) {
		glm::vec3 vec = _steps[i + 1].pos - _steps[i].pos;
		vec *= mean / glm::length(_steps[i + 1].pos - _steps[i].pos);
		_steps[i + 1].pos = _steps[i].pos + vec;
	}
}