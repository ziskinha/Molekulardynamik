/*
 * Particle.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "Particle.h"

#include <iostream>
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "io/Logger.h"


namespace md {
	/// -----------------------------------------
	/// \brief Particle Class Methods
	/// -----------------------------------------
	Particle::Particle(const int type):
		position({}),
		velocity({}),
		force({}),
		old_force({}),
		mass(0),
		type(type) {
		// spdlog::info("Particle generated!");
	}

	Particle::Particle(const vec3& position, const vec3& velocity, const double mass, const int type)
		: position(position), velocity(velocity), force({}), old_force({}), mass(mass), type(type) {
		// spdlog::info("Particle generated by copy 2!");
	}

	Particle::Particle(const Particle& other)
		: position(other.position), velocity(other.velocity), force(other.force),
		  old_force(other.old_force), mass(other.mass), type(other.type) {
		// spdlog::info("Particle generated by copy!");
	}

	Particle::~Particle() {
		// spdlog::info("Particle destructed!");;
	}

	std::string Particle::to_string() const {
		std::stringstream stream;
		using ::operator<<;
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


	/// -----------------------------------------
	/// \brief ParticleContainer Class Methods
	/// -----------------------------------------
	ParticleContainer::ParticleContainer(const std::vector<Particle>& particles):
		particles(particles) {
	}

	ParticleContainer::ParticleContainer(std::vector<Particle>&& particles):
		particles(std::move(particles)) {
	}

	void ParticleContainer::add_cuboid(const vec3& origin, const std::array<u_int32_t, 3>& num_particles,
	                                   const vec3& init_v, double thermal_v, double width, double mass, int dimension, int type)
	{
		particles.reserve(particles.size() + num_particles[0] * num_particles[1] * num_particles[2]);

		for (unsigned int x = 0; x < num_particles[0]; ++x) {
			for (unsigned int y = 0; y < num_particles[1]; ++y) {
				for (unsigned int z = 0; z < num_particles[2]; ++z) {
					vec3 pos = origin + vec3({
						x * width, y * width, z * width
					});

					vec3 vel = init_v + maxwellBoltzmannDistributedVelocity(thermal_v, dimension);

					particles.emplace_back(pos, vel, mass, type);
				}
			}
		}
	}

	void ParticleContainer::add_particles(const std::vector<Particle>& particles) {
		this->particles.insert(this->particles.end(), particles.begin(), particles.end());
	}

	size_t ParticleContainer::size() const {
		return particles.size();
	}

	Particle& ParticleContainer::operator[](const size_t index) {
		return particles[index];
	}

	const Particle& ParticleContainer::operator[](const size_t index) const {
		return particles[index];
	}

	ParticleContainer ParticleContainer::operator+(const ParticleContainer& other) const {
		ParticleContainer result(*this);
		result.particles.insert(result.particles.end(), other.particles.begin(), other.particles.end());
		return result;
	}

}

