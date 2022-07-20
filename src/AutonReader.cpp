#include "AutonReader.h"

#include "Utils.h"

AutonReader::AutonReader(std::string filepath) {

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

}

AutonReader::Step AutonReader::getNextStep() {

	if (_line == nullptr) {
		_line = _auton->FirstChildElement();
	}
	else {
		_line = _line->NextSiblingElement();
	}

	if (_line == nullptr) {
		return Step{ NULL, NULL, true };
	}

	if (std::strcmp(_line->Name(), "look") == 0) {

		double x;
		double y;
		double z;

		_line->QueryDoubleAttribute("x", &x);
		_line->QueryDoubleAttribute("y", &y);
		_line->QueryDoubleAttribute("z", &z);

		_looking = Vec3{ x, y, z };

		return getNextStep();
	}
	else if (std::strcmp(_line->Name(), "move") == 0) {

		double x;
		double y;
		double z;

		_line->QueryDoubleAttribute("x", &x);
		_line->QueryDoubleAttribute("y", &y);
		_line->QueryDoubleAttribute("z", &z);

		 Vec3 pos{ x, y, z };

		 Quaternion rot = eulerToQuat(lookAt(pos, _looking));

		 return Step{ pos, rot };
	}
}

std::string AutonReader::getAutonName()
{
	return std::string(_auton->Attribute("name"));
}
