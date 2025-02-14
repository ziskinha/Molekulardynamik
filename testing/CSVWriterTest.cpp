#include "../src/io/Output/CSVWriter.h"
#include <gtest/gtest.h>
 

TEST(CSVWriterTest, FileCreation) {
    md::io::CSVWriter writer(5);
    writer.~CSVWriter();
    std::ifstream file("statistics.csv");
    ASSERT_TRUE(file.is_open()); 
    file.close();
    std::remove("statistics.csv");  

}

TEST(CSVWriterTest, HeaderWrite) {
    md::io::CSVWriter writer(3);  
    writer.~CSVWriter();
    std::ifstream file2("statistics.csv");
    ASSERT_TRUE(file2.is_open()); 

    std::string header;
    std::getline(file2, header);

    EXPECT_EQ(header, "time&vel&dens/bins,1,2,3");
    file2.close();
    std::remove("statistics.csv");  

}

TEST(CSVWriterTest, WriteData) {
    md::io::CSVWriter writer(3);
    
    std::vector<double> velocities = {1.1, 2.2, 3.3};
    std::vector<double> densities = {4.4, 5.5, 6.6};
    writer.writeData(velocities, densities, 10.0);  
    writer.~CSVWriter();
    std::ifstream file("statistics.csv");
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line); 
    std::getline(file, line); 
    EXPECT_EQ(line, "10,");

    std::getline(file, line); 
    EXPECT_EQ(line, "v,1.1,2.2,3.3");

    std::getline(file, line); 
    EXPECT_EQ(line, "d,4.4,5.5,6.6");
    file.close();
    std::remove("statistics.csv");  
}