#pragma once

Vec3 lookAt(Vec3 pos, Vec3 target) {
	return Vec3{ 
		atan2(target.y - pos.y, target.z - pos.z),
		atan2(target.z - pos.z, target.x - pos.x),
		atan2(target.y - pos.y, target.x - pos.x)
	};
}

Quaternion eulerToQuat(Vec3 rot) {

    double cx = cos(rot.x * 0.5);
    double sx = sin(rot.x * 0.5);
    double cy = cos(rot.y * 0.5);
    double sy = sin(rot.y * 0.5);
    double cz = cos(rot.z * 0.5);
    double sz = sin(rot.z * 0.5);

    return Quaternion{
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sz * sy * sz
    };
}

Vec3 geoToCoords(const char* latS, const char* lonS, double alt) {

	static const int earthRadius = 6378137;

	char* hemi;
	double lat = strtod(latS, &hemi);
	if (strcmp(hemi, "S") == 0) {
		lat += 180;
	}

	double lon = strtod(lonS, &hemi);
	if (strcmp(hemi, "W") == 0) {
		lon += 180;
	}

	double distance = alt + earthRadius;
	return Vec3{ cos(lon) * distance * cos(lat), sin(lon) * distance * cos(lat), distance * sin(lat) };
}