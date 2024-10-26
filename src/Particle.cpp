/*
 * Particle.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "Particle.h"

#include <iostream>
#include "utils/ArrayUtils.h"

Particle::Particle(int type):
	type(type),
	position({}),
	velocity({}),
	force({}),
	old_force({}),
	mass(0)
{
	std::cout << "Particle generated!" << std::endl;
}

Particle::Particle(vec3 position, vec3 velocity, double mass, int type)
	: position(position), velocity(velocity), force({}), old_force({}), mass(mass), type(type) {
	std::cout << "Particle generated by copy!" << std::endl;
}

Particle::Particle(const Particle& other)
	: position(other.position), velocity(other.velocity), force(other.force),
	  old_force(other.old_force), mass(other.mass), type(other.type) {
	std::cout << "Particle generated by copy!" << std::endl;
}


Particle::~Particle() {
	std::cout << "Particle destructed!" << std::endl;
}



std::string Particle::to_string() const {
	std::stringstream stream;
	stream << "Particle: X:" << position << " v: " << velocity << " f: " << force
		<< " old_f: " << old_force << " type: " << type;
	return stream.str();
}


bool Particle::operator==(const Particle& other) const {
	return position == other.position &&
		   velocity == other.velocity &&
		   force == other.force &&
		   old_force == other.old_force &&
		   mass == other.mass &&
		   type == other.type;
}

std::ostream& operator<<(std::ostream& stream, Particle& p) {
	stream << p.to_string();
	return stream;
}
