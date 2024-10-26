/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>


using vec3 = std::array<double, 3>;
struct Particle {
	explicit Particle(int type = 0);
	Particle(vec3 position, vec3 velocity, double mass, int type = 0);
	Particle(const Particle& other);
	~Particle();

	[[nodiscard]] std::string to_string() const;
	bool operator==(const Particle& other) const;

	vec3 position;
	vec3 velocity;
	vec3 force;
	vec3 old_force;
	double mass;
	int type;
};

std::ostream& operator<<(std::ostream& stream, Particle& p);
