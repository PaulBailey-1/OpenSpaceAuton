#pragma once
#include <string>
#include <iostream>

#include "tinyxml2.h"
#include "MathTypes.h"

class AutonReader {
public:

	struct Step {
		Vec3 pos;
		Quaternion rot;
		bool end = false;
	};

	AutonReader(std::string filepath);

	Step getNextStep();

	std::string getAutonName();

private:

	tinyxml2::XMLDocument _file;
	tinyxml2::XMLElement* _auton;
	tinyxml2::XMLElement* _line;

	Vec3 _looking;

};