#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "env/Environment.h"
#include "env/Thermostat.h"
#include "io/Logger/Logger.h"
#include "io/Output/CheckpointWriter.h"

#define OUTPUT_DIR "output"

/**
 * @brief Namespace containing classes and functions for output writing.
 */
namespace md::io {

    enum class OutputFormat { VTK, XYZ };

    /**
     * @brief Struct used to return arguments read from the terminal.
     */
    struct ProgramArguments {
        env::Environment env;
        env::Boundary boundary;
        env::Thermostat thermostat;
        OutputFormat output_format;
        std::string output_baseName;
        std::string force;
        bool benchmark;
        bool override;
        double duration;
        double dt;
        double cutoff_radius;
        int write_freq;
        unsigned int temp_adj_freq;
    };

    /**
     * @brief Logs the general simulation info.
     * @param args
     */
    inline void log_arguments(const ProgramArguments& args) {
        SPDLOG_INFO(
                "General simulation info:\n"
                "       output name:         {}\n"
                "       duration:            {}\n"
                "       dt:                  {}\n"
                "       write_freq:          {}\n"
                "       particles:           {}\n"
                "       benchmark:           {}\n"
                "       override:            {}\n"
                "       output_format:       {}",
                args.output_baseName, args.duration, args.dt, args.write_freq,
                args.env.size(), args.benchmark ? "true" : "false",
                args.override ? "true" : "false", args.output_format == OutputFormat::XYZ ? "XYZ" : "VTK");
    }

    /**
     * @brief Abstract base class for output writers.
     * OutputWriter is the superclass for VTKWriter and XYZWriter.
     */
    class OutputWriterBase {
       public:
        /**
         * @brief Constructs an OutputWriter with a specified file name.
         * @param file_name The name of the output file.
         * @param allow_delete if output folder contains files, allow for deletion
         */
        explicit OutputWriterBase(std::string file_name, bool allow_delete);
        virtual ~OutputWriterBase() = default;

        /**
         * @brief Outputs particle data at a given iteration.
         * @param environment
         * @param iteration The current simulation iteration.
         */
        virtual void plot_particles(const env::Environment& environment, int iteration) = 0;

       protected:
        const std::string file_name;
    };

    /**
     * @brief Checks if the filename ends with the specific extension.
     * @param filename The filename of the file to check.
     * @param extension The file extension to check for, e.g., ".txt" or ".xml"
     * @return true if the filename ends with the specific extension, false otherwise.
     */
    bool check_format(const std::string& filename, const std::string& extension);

    /**
     * @brief Create an output writer.
     * @param outputFileBaseName
     * @param output_format A boolean flag indicating whether to use the VTK output format or XYZ.
     * @param allow_delete
     * @return A pointer to an OutputWriter object.
     */
    std::unique_ptr<OutputWriterBase> create_writer(const std::string& outputFileBaseName, OutputFormat output_format, bool allow_delete);

    /**
     * @brief Create an checkpoint output writer.
     * @return A pointer to an CheckpointWriter object.
     */
    std::unique_ptr<CheckpointWriter> create_checkpoint_writer();

    /**
     * @brief Reads an input file depending on its format.
     * @param filename
     * @param args
     */
    void read_file(const std::string& filename, ProgramArguments& args);
}  // namespace md::io
