/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>

#include "IOStrategy.h"

namespace md::io {
    class XYZWriter : public OutputWriterBase {
       public:
        explicit XYZWriter(std::string fileName, bool allow_delete);
        ~XYZWriter() override;

        void plot_particles(const Environment & env, int iteration) override;
    };
}  // namespace md::io
