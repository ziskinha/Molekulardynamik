#pragma once
#include <cmath>
#include <functional>
#include <limits>
#include <utility>
#include "Common.h"
#include "utils/ArrayUtils.h"
#include "Particle.h"

#define NO_FORCE_CUTOFF std::numeric_limits<double>::max()
#define FORCE_CUTOFF_AUTO std::numeric_limits<double>::min()

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {

    struct ForceBase {
        double cutoff = FORCE_CUTOFF_AUTO;
    };


    // TODO fix description
    /**
    * @brief Returns a Force object representing the Lennard-Jones potential between two particles.
    * @param epsilon Depth of the potential well.
    * @param sigma Distance at which the potential reaches zero.
    * @param cutoff_radius The cutoff radius for the force.
    * @return The Force object configured for Lennard-Jones potential.
    */
    struct LennardJones : ForceBase{
        LennardJones() = default;
        LennardJones(const double epsilon, const double sigma, const double cutoff)
        : ForceBase(cutoff), epsilon(epsilon), sigma(sigma) {}
        double epsilon = 5;
        double sigma = 1;
    };

    // TODO fix description
    /**
    * @brief Returns a Force object that represents the inverse-square force between two particles.
    * @param pre_factor Scaling factor for the magnitude of the force (e.g., for gravity or Coulomb's law).
    * @param cutoff_radius The cutoff radius for the force.
    * @return The Force object configured for an inverse-square force.
    */
    struct InverseSquare : ForceBase{
        InverseSquare() = default;
        InverseSquare(const double pre_factor, const double cutoff)
        : ForceBase(cutoff), pre_factor(pre_factor) {}
        double pre_factor = 1;
    };


    using ForceType = std::variant<LennardJones, InverseSquare>;


    class Force {
    public:
        using ForceFunc = std::function<vec3(const vec3&, const Particle&, const Particle&)>;

        Force();

        /**
         * @brief Constructs a Force object with a specified force function and cutoff.
         * @param force_func The function to calculate force between particles.
         * @param cutoff The cutoff radius for the force.
         */
        explicit Force(ForceFunc force_func, double cutoff = NO_FORCE_CUTOFF);

        /**
         * @brief Calculates the force between two particles.
         * @param diff Distance vector between the two particles.
         * @param p1 The first particle.
         * @param p2 The second particle.
         * @return The force between the two particles.
         */
        vec3 operator()(const vec3& diff, const Particle& p1, const Particle& p2) const;

        /**
         * @brief Retrieves the cutoff radius.
         * @return The cutoff radius.
         */
        [[nodiscard]] double cutoff() const;

    private:
        double cutoff_radius;    ///< The cutoff radius for the calculations.
        ForceFunc force_func {}; ///< The force function used for the calculations.
    };



    class ForceManager {
        using ParticleType = int;
        using ForceKey = std::pair<ParticleType, ParticleType>;

        struct ForceKeyHash {
            template <typename T1, typename T2>
            std::size_t operator()(const std::pair<T1, T2>& key) const {
                return std::hash<T1>()(key.first) ^ (std::hash<T2>()(key.second) << 1);
            }
        };
    public:
        ForceManager();

        void init();
        void add_force(const ForceType& force, int particle_type);
        vec3 evaluate(const vec3 &diff, const Particle& p1, const Particle& p2) const;
        double cutoff() const;

    private:
        static Force mix_forces(const ForceType& force1, const ForceType& force2);
        std::unordered_map<ParticleType, ForceType> force_types;
        std::unordered_map<ForceKey, Force, ForceKeyHash> forces;
        double cutoff_radius;
    };

}  // namespace md::env
