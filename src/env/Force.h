#pragma once
#include <cmath>
#include <functional>
#include <limits>
#include <utility>
#include <variant>
#include "Common.h"
#include "utils/ArrayUtils.h"
#include "Particle.h"

#define NO_FORCE_CUTOFF std::numeric_limits<double>::max()
#define FORCE_CUTOFF_AUTO std::numeric_limits<double>::min()

namespace md::env {

    /**
     * @brief Structure for defining force-related configurations.
     */
    struct ForceBase {
        double cutoff = FORCE_CUTOFF_AUTO;
    };

    /**
     * @brief Represents the Lennard-Jones force between two particles.
     */
    struct LennardJones : ForceBase{
        /**
         * @brief Default constructor.
         */
        LennardJones() = default;

        /**
         * @brief Constructs a Lennard-Jones object with parameters.
         * @param epsilon
         * @param sigma
         * @param cutoff
         */
        LennardJones(const double epsilon, const double sigma, const double cutoff)
        : ForceBase(cutoff), epsilon(epsilon), sigma(sigma) {}
        double epsilon = 5;
        double sigma = 1;
    };

    /**
     * @brief Represents an inverse-square force between two particles.
     */
    struct InverseSquare : ForceBase{
        /**
         * @brief Default constructor.
         */
        InverseSquare() = default;

        /**
         * @brief Constructs an inverse-square force object with parameters.
         * @param pre_factor
         * @param cutoff
         */
        InverseSquare(const double pre_factor, const double cutoff)
        : ForceBase(cutoff), pre_factor(pre_factor) {}
        double pre_factor = 1;
    };


    using ForceType = std::variant<LennardJones, InverseSquare>;


    class Force {
    public:
        using ForceFunc = std::function<vec3(const vec3&, const Particle&, const Particle&)>;

        /**
         * @brief Default constructor.
         */
        Force();

        /**
         * @brief Constructs a Force object.
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


    /**
     * @brief Manages forces of particles with different types.
     */
    class ForceManager {
        using ParticleType = int;
        using ForceKey = std::pair<ParticleType, ParticleType>;

        /**
         * @brief Hash to use pairs of particles types as keys.
         */
        struct ForceKeyHash {
            /**
             * @brief Hash for a pair of elements.
             * @tparam T1
             * @tparam T2
             * @param key The pair of elements to hash.
             * @return Value representing the computed hash.
             */
            template <typename T1, typename T2>
            std::size_t operator()(const std::pair<T1, T2>& key) const {
                return std::hash<T1>()(key.first) ^ (std::hash<T2>()(key.second) << 1);
            }
        };
    public:
        /**
         * @brief Default constructor.
         */
        ForceManager();

        /**
         * @brief Initializes the ForceManager. It computes the corresponding force objects for all particle type
         * combinations.
         *
         * @throws std::runtime_error if an unsupported force type is encountered.
         */
        void init();

        /**
         * @brief Adds a force configuration for a specific particle type.
         * @param force The force configuration to add.
         * @param particle_type The type of particle for which the force should apply.
         */
        void add_force(const ForceType& force, int particle_type);

        /**
         * @brief TODO: description
         * @param diff
         * @param p1
         * @param p2
         * @return
         */
        vec3 evaluate(const vec3 &diff, const Particle& p1, const Particle& p2) const;

        /**
         * @brief Retrieves the cutoff radius.
         * @return The cutoff radius.
         */
        double cutoff() const;

    private:
        /**
         * @brief Combines two forces into a single force object for two different types of forces.
         * For Lennard-Jones type, Lorentz-Berthelot mixing rule is used.
         * For Inverse-Squared, TODO?
         *
         * @param force1 The first force to combine.
         * @param force2 The second force to combine.
         * @return A new "Force" object representing the combination of the two forces.
         *
         * @throws std::invalid_argument if the two forces are of incompatible types (e.g. one "Lennard-Jones" and
         * one "InverseSquare", as this is not supported.
         */
        static Force mix_forces(const ForceType& force1, const ForceType& force2);

        std::unordered_map<ParticleType, ForceType> force_types;   ///< Map of particle types to force configurations.
        std::unordered_map<ForceKey, Force, ForceKeyHash> forces;  ///< Map of force key pairs to mixed forces.
        double cutoff_radius;  ///< The cutoff radius.
    };

}  // namespace md::env
