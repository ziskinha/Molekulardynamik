
#include "ParticleContainer.h"


// ParticleContainer::ParticleContainer(const Container& container) {
// 	static_assert(std::is_same_v<typename Container::value_type, Particle>, "Container must hold Particle elements");
// 	particles.assign(container.begin(), container.end());
// }

ParticleContainer::ParticleContainer(const std::vector<Particle> & particles):
	particles(particles)
{}

size_t ParticleContainer::size() const {
	return particles.size();
}


