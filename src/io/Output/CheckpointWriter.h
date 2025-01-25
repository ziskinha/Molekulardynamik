#include "env/Environment.h"
#include <filesystem>

namespace md::io {
    class CheckpointWriter {
    public:
        /**
         * @brief Constructs a new CheckpointWriter object.
         * @param file_base_name base name of the checkpoint files (default: "checkpoint").
         * @param output_dir name of the output directory (default: "Checkpoint").
         */
        explicit CheckpointWriter(std::string file_base_name = "checkpoint", std::string output_dir = "Checkpoint");
        virtual ~CheckpointWriter() = default;

        /**
         * @brief writes particle information to a checkpoint file.
         * @param env the environment of the particles.
         * @param num identification number of the checkpoint file.
         */
        void write_checkpoint_file(env::Environment& env, size_t num);

    private:
        const std::string file_base_name;  ///< base name of the checkpoint files.
        const std::string output_dir;      ///< name of the output directory.
    };
} // namespace md::io