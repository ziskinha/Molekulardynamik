#include "CSVWriter.h"
#include <spdlog/spdlog.h>

namespace md::io{
CSVWriter::CSVWriter(int bins) : bins{bins} {
   
    file = std::ofstream{"statistics.csv"};
    if (!file.is_open()) {
        spdlog::error("Failure while opening statistics.csv");
        file.close();
        exit(1);
    }

    file << "time&vel&dens/bins,";

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

void CSVWriter::writeData(std::vector<double> &vel,std::vector<double> &dens, double time) {
    file << time << ",";
    file << "\n";
    file <<"v" <<",";
    for(int i = 0; i < bins; i++) {
        file << vel[i];
        if(i < bins - 1) {
            file << ",";
        }
        else {
            file << "\n";
        }
    }
        file <<"d" <<",";
         for(int i = 0; i < bins; i++) {
        file << dens[i];
        if(i < bins - 1) {
            file << ",";
        }
        else {
            file << "\n";
        }
    }
    
}
}