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

			_target = geoToCoordinates(parseCoordinates(_line->GetText()));

		}
		else if (std::strcmp(_line->Name(), "move") == 0) {

			LookType lookType;
			const char* look;
			if (_line->QueryStringAttribute("look", &look) == tinyxml2::XMLError::XML_NO_ATTRIBUTE) {
				look = "target";
			}

			if (strcmp(look, "forward") == 0) {
				lookType = FORWARD;
			}
			else if (strcmp(look, "backward") == 0) {
				lookType = BACKWARD;
			}
			else if (strcmp(look, "down") == 0) {
				lookType = DOWN;
			}
			else if (strcmp(look, "target") == 0) {
				lookType = TARGET;
			}

			double speed = 0.0;
			_line->QueryDoubleAttribute("speed", &speed);

			if (speed == 0.0) {
				speed = _speed;
			}

			double delay = 0.0;
			_line->QueryDoubleAttribute("delay", &delay);

			int factor = 0;
			_line->QueryIntAttribute("factor", &factor);

			bool altSpeed = true;
			_line->QueryBoolAttribute("altSpeed", &altSpeed);

			Geo point = parseCoordinates(_line->GetText());
			glm::vec3 pos = geoToCoordinates(point);

			_steps.push_back(Step(Step::StepType::CAMERA));
			_steps.back().pos = pos;
			_steps.back().lookType = lookType;
			_steps.back().target = _target;
			_steps.back().speed = speed;
			_steps.back().interpolate = factor;
			_steps.back().lookType = lookType;
			_steps.back().geo = point;
			_steps.back().altSpeed = altSpeed;
			_steps.back().time = delay;
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

			int factor = 0;
			_line->QueryIntAttribute("factor", &factor);

			const char* coords = _line->GetText();

			std::string str(coords);
			std::stringstream ss(str);
			std::string coord;
			double riseOffset = 0.0;

			int startIndex = _steps.size();

			while (ss >> coord) {

				Geo point = parseCoordinates(coord.c_str(), true, ',');
				point.alt += riseOffset;
				glm::vec3 pos = geoToCoordinates(point);

				_steps.push_back(Step(Step::StepType::CAMERA));
				_steps.back().pos = pos;
				_steps.back().lookType = lookType;
				_steps.back().speed = speed;
				_steps.back().geo = point;
				_steps.back().interpolate = factor;

				riseOffset += rise;
			}

		} else if (std::strcmp(_line->Name(), "simSpeed") == 0) {
			int sim = 0;
			_line->QueryIntAttribute("deltaSpeed", &sim);

			_steps.push_back(Step(Step::DELTATIME));
			_steps.back().deltaTime = sim;
		}
		else {
			printf("Error: could not read xml\n");
		}
	}
}

void AutonReader::interpolatePoints() {

	int startIndex = 0;
	int endIndex = _steps.size() - 1;
	int loop = 0;

	for (int i = 0; i < _steps.size(); i++) {
		if (loop > 0 && _steps[i].interpolate == 0) {
			endIndex = i - 1;
			break;
		}
		if (_steps[i].interpolate > 0 && loop == 0) {
			startIndex = i;
			loop = _steps[i].interpolate;
		}
	}

	int endOffset = _steps.size() - 1 - endIndex;
	for (int j = 0; j < loop; j++) {
		for (int i = startIndex; i < _steps.size() - 1 - endOffset; i += 2) {

			Geo mean;
			Step nextStep = getNextCamera(i);
			mean.lat = (_steps[i].geo.lat + nextStep.geo.lat) / 2;
			mean.lon = (_steps[i].geo.lon + nextStep.geo.lon) / 2;
			mean.alt = (_steps[i].geo.alt + nextStep.geo.alt) / 2;

			Step newStep = _steps[i];
			newStep.geo = mean;
			newStep.pos = geoToCoordinates(mean);

			newStep.rot = glm::mix(nextStep.rot, _steps[i].rot, 0.5f);

			_steps.insert(_steps.begin() + i + 1, newStep);
		}
	}
}

void AutonReader::computeSpeeds() {

	for (int i = 0; i < _steps.size() - 1; i++) {
		if (!_steps[i].time > 0 && _steps[i].speed > 0.0) {

			if (_steps[i].altSpeed) {
				_steps[i].speed *= pow(2, _steps[i].geo.alt * 0.00003);
			}

			double distance = glm::length(getNextCamera(i).pos - _steps[i].pos);
			_steps[i].time = distance / _steps[i].speed;
		}
	}
}

void AutonReader::computeView() {
	for (int i = 0; i < _steps.size(); i++) {
		if (_steps[i].lookType == LookType::TARGET) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i].target - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else if (_steps[i].lookType == LookType::FORWARD) {
			if (i != _steps.size() - 1) {
				Geo geoTarget = getNextCamera(i).geo;
				geoTarget.alt = _steps[i].geo.alt;
				glm::vec3 target = geoToCoordinates(geoTarget);

				_steps[i].rot = glm::quatLookAt(glm::normalize(target - _steps[i].pos), glm::normalize(_steps[i].pos));
			}
			else {
				_steps[i].rot = _steps[i - 1].rot;
			}
			
		}
		//else if (_steps[i].lookType == LookType::FORWARD && i < _steps.size() - 10) {
		//	_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[(int) (floor((i + 10) / 10.0) * 10)].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
		//}
		else if (_steps[i].lookType == LookType::BACKWARD && i >= 10) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i - 10].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else if (_steps[i].lookType == LookType::BACKWARD) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(_steps[i - 1].pos - _steps[i].pos), glm::normalize(_steps[i].pos));
		}
		else if (_steps[i].lookType == LookType::DOWN) {
			_steps[i].rot = glm::quatLookAt(glm::normalize(-_steps[i].pos), glm::normalize(_steps[i].pos));
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
	_auton->QueryInt64Attribute("simTime", &time);
	return time;
}

Geo AutonReader::parseCoordinates(const char* coords, bool flip, char delim) {

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

	Geo point;
	point.lat = std::stod(lats);
	point.lon = std::stod(lons);
	point.alt = std::stod(alts);
	
	return point;
}

glm::vec3 AutonReader::geoToCoordinates(Geo point) {

	const int earthRadius = 6378137;
	//const int earthRadius = 3389500;

	point.lat *= M_PI / 180;
	point.lon *= M_PI / 180;

	double distance = point.alt + earthRadius;
	return glm::vec3(cos(point.lon) * distance * cos(point.lat), sin(point.lon) * distance * cos(point.lat), distance * sin(point.lat));
}

AutonReader::Step AutonReader::getNextCamera(int current) {
	for (int i = current + 1; i < _steps.size(); i++) {
		if (_steps[i].type == Step::CAMERA) {
			return _steps[i];
		}
	}
}
