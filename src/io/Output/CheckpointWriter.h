#include "env/Environment.h"
#include <filesystem>

namespace md::io {
    class CheckpointWriter {
    public:
        explicit CheckpointWriter();
        virtual ~CheckpointWriter() = default;

        void write_checkpoint_file(env::Environment& env, size_t num);

    protected:
        const std::string file_base_name;
    };
} // namespace md::io