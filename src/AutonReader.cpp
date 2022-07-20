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
		return Step( Vec3(), Quaternion(), true );
	}

	if (std::strcmp(_line->Name(), "look") == 0) {

		const char* lat;
		const char* lon;
		double alt;

		_line->QueryStringAttribute("lat", &lat);
		_line->QueryStringAttribute("lon", &lon);
		_line->QueryDoubleAttribute("alt", &alt);

		_looking = geoToCoords(lat, lon, alt);

		return getNextStep();
	}
	else if (std::strcmp(_line->Name(), "move") == 0) {

		const char* lat;
		const char* lon;
		double alt;

		_line->QueryStringAttribute("lat", &lat);
		_line->QueryStringAttribute("lon", &lon);
		_line->QueryDoubleAttribute("alt", &alt);

		Vec3 pos = geoToCoords(lat, lon, alt);
		Vec3 eul = lookAt(pos, _looking);

		Quaternion rot = eulerToQuat(lookAt(pos, _looking));

		 return Step(pos, rot, false);
	}
	else {
		printf("Error: could not read xml\n");
	}
}

std::string AutonReader::getAutonName()
{
	return std::string(_auton->Attribute("name"));
}