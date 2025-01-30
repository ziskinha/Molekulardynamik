#include <fstream>
#include <vector>

namespace md::io{

class CSVWriter {
private:
     std::ofstream file;
     int bins;

public:
     /**
      * @brief  Creates a csv file and enables writing in it.
      */
      CSVWriter(int bins);
     /**
      * @brief The file is closed after object is destructed.
      */
     ~CSVWriter();
     /**
      * @brief Writes data for a current time in simulation.
      *
      * @param data Data of particles.
      * @param time The time for which the data is specified.
      */
     void writeData(std::vector<double>& vel,std::vector<double>& dens, double time);
};
};