#include "all_headers.h"

void Autopilot::set_start(glm::vec3 pos) {

	start = scale_position(pos);

	if (point_exists(start)) {
		std::cout << "START SET\n";
	}
}


void Autopilot::set_destination(glm::vec3 pos) {

	destination = scale_position(pos);

	if (point_exists(destination)) {
		std::cout << "DESTINATION SET\n";
	}
}


void Autopilot::start_autopilot() {
	generate_path();

	std::thread test(&Autopilot::stop_autopilot, this);
	test.detach();
}

void Autopilot::stop_autopilot() {

	//just test
	while (paths.size() > 0) {
		std::vector<_vec2> sub_path = *paths.begin();
		paths.erase(paths.begin());

		while (sub_path.size() > 0) {
			_vec2 next_pos = *sub_path.begin();
			sub_path.erase(sub_path.begin());

			position->x = next_pos.x;
			position->z = next_pos.z;
			std::cout << "next_pos: " << position->x << " " << position->z << "\n";

			Sleep(100);
		}
	}
}

void Autopilot::generate_path() {

	//Check if Start & Destination are set and on a street
	if (StreetMap->count(start) == 1 && StreetMap->count(destination) == 1) {

		if (StreetMap->find(start)->second <= STREET_IDENTIFIER_THRESHOLD
			&& StreetMap->find(destination)->second <= STREET_IDENTIFIER_THRESHOLD) {

		}
		else { return; }
	}
	else { return; }

	paths.clear();
	_vec2 pos = start;
	std::vector<_vec2> sub_path;

	while (pos != destination) {
		//dummy path
		pos = nearest_point_in_direction(pos, destination);
		sub_path.push_back(pos);

		if (sub_path.size() == MAX_SUBPATH_LENGTH) {
			paths.push_back(sub_path);
			sub_path.clear();
		}

		if (pos == destination) {
			paths.push_back(sub_path);
		}
	}		
}

//doesn't check street threshold
_vec2 Autopilot::nearest_point_in_direction(_vec2 pos, _vec2 dir) {
	_vec2 result;
	_vec2 unit = { float(m_Terrain->X_SCALAR), float(m_Terrain->Z_SCALAR) };

	if (pos == dir) {
		return pos;
	}

	if (dir.x > pos.x) {
		result.x = pos.x + unit.x; // = increment_pos(pos, 1, 0); would do the same
	}
	else if (dir.x < pos.x) {
		result.x = pos.x - unit.x;
	}
	else {
		result.x = pos.x;
	}


	if (dir.z > pos.z) {
		result.z = pos.z + unit.z;
	}
	else if (dir.z < pos.z) {
		result.z = pos.z - unit.z;
	}
	else {
		result.z = pos.z;
	}

	return result;
}


_vec2 Autopilot::increment_pos(_vec2 pos, int x, int z) {
	_vec2 unit = { float(m_Terrain->X_SCALAR), float(m_Terrain->Z_SCALAR) };

	pos.x = pos.x + float(x) * unit.x;
	pos.z = pos.z + float(z) * unit.z;

	return pos;
}

_vec2 Autopilot::scale_position(_vec2 pos) {

	_vec2 result;

	result.x = float((int(pos.x) + (m_Terrain->X_SCALAR / 2)) / m_Terrain->X_SCALAR * m_Terrain->X_SCALAR);
	result.z = float((int(pos.z) + (m_Terrain->Z_SCALAR / 2)) / m_Terrain->Z_SCALAR * m_Terrain->Z_SCALAR);

	return result;
}

_vec2 Autopilot::scale_position(glm::vec3 pos) {
	_vec2 result;

	result.x = pos.x;
	result.z = pos.z;

	return scale_position(result);
}


bool Autopilot::point_exists(_vec2 pos) {
	if (StreetMap->count(pos) == 1) {
		return true;
	}
	return false;
}


bool Autopilot::point_meets_threshold(_vec2 pos) {
	if (point_exists(pos)) {
		if (StreetMap->find(pos)->second <= STREET_IDENTIFIER_THRESHOLD) {
			return true;
		}
	}
	return false;
}