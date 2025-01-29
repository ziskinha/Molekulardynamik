#include "CSVWriter.h"
#include <io/Logger/Logger.h>

namespace md::io{
CSVWriter::CSVWriter(int bins, std::string filename) : bins{bins} {
   
    file = std::ofstream{ filename + ".csv"};
    if (!file.is_open()) {
        SPDLOG_ERROR("Failure while opening {}.csv",filename);
        exit(1);
    }else {
        SPDLOG_INFO("{}.csv was opened",filename);
    }

    file << "time,";

    for(int i = 1; i <= bins; i++) {
        file << i;
        if(i<bins) {
            file << ",";
        }
        else {
            file << "\n";
        }
    }
}

CSVWriter::~CSVWriter() {
    file.close();
}

void CSVWriter::writeData(std::vector<double> &values, double time) {
    file << time << ",";
    for(int i = 0; i < bins; i++) {
        file << values[i];
        if(i < bins - 1) {
            file << ",";
        }
        else {
            file << "\n";
        }
    }
   
}
}