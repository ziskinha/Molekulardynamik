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

/**
 * @brief Struct representing a particle.
 */
struct Particle {
    /**
     * @brief Constructor that creates a particle with a specified type.
     * @param type
     */
	explicit Particle(int type = 0);

    /**
     * @brief Constructor that creates a particle with position, velocity, mass and type.
     * @param position
     * @param velocity
     * @param mass
     * @param type (default value is 0).
     */
	Particle(vec3 position, vec3 velocity, double mass, int type = 0);

    /**
     * @brief Copy constructor that creates a new particle from an existing one.
     * @param other
     */
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


