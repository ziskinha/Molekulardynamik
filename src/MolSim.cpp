
#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"
#include "outputWriter/VTKWriter.h"

#include <iostream>
#include <list>
#include <cmath>

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();

/**
 * plot the particles to a xyz-file and a vtk-file
 */
void plotParticles(int iteration);

constexpr double start_time = 0;
//constexpr double end_time = 1000;
//constexpr double delta_t = 0.014;
double end_time = 0.0;
double delta_t = 0.0;

// TODO: what data structure to pick?
std::list<Particle> particles;

int main(int argc, char *argsv[]) {

  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 4) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./MolSim filename end_time delta_t" << std::endl;
    return 0;
  }

  FileReader fileReader;
  fileReader.readFile(particles, argsv[1]);

  end_time = std::stod(argsv[2]);
  delta_t = std::stod(argsv[3]);
  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    // calculate new x
    calculateX();
    // calculate new f
    calculateF();
    // calculate new v
    calculateV();

    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(iteration);
    }

    // for (auto &p : particles)
    // {
    // std::cout << p.getF() << std::endl;
    //   std::cout << p.getX() << std::endl;
    //   std::cout << p.getF() << std::endl;
    //   std::cout << std::endl;
    //
    // }

    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
  }

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}

void calculateF() {
  std::list<Particle>::iterator iterator;
  iterator = particles.begin();

  for (auto &p1 : particles) {
    std::array<double, 3> new_F = {0};
    for (auto &p2 : particles) {
      // @TODO: insert calculation of forces here!
      if (p1 == p2) continue;
      double dist  = ArrayUtils::L2Norm(p1.getX() - p2.getX());
      double F_mag =  p1.getM() * p2.getM() / pow(dist, 3);
      new_F = new_F + F_mag * dist * (p2.getX() - p1.getX());
    }
    p1.setF(new_F);
  }
}

void calculateX() {
  for (auto &p : particles) {
    // @TODO: insert calculation of position updates here!
    std::array<double, 3> new_x = p.getX() + (delta_t * p.getV()) + (pow(delta_t, 2)/(2*p.getM()) *  p.getOldF());
    p.setX(new_x);
  }
}

void calculateV() {
  for (auto &p : particles) {
    // @TODO: insert calculation of veclocity updates here!
    std::array<double, 3> new_v = p.getV() + delta_t/2/p.getM() * (p.getF() + p.getOldF()) ;
    p.setV(new_v);
  }
}

void plotParticles(int iteration) {
  //xyz output
  std::string out_name_xyz("MD_xyz");
  outputWriter::XYZWriter writer_XYZ;
  writer_XYZ.plotParticles(particles, out_name_xyz, iteration);

  //vtk output
  std::string out_name_vtk("MD_vtk");
  outputWriter::VTKWriter writer_VTK;
  writer_VTK.initializeOutput(int(particles.size()));
  // @TODO: Put loop into VTKWriter::plotParticle?
  for (auto &p : particles) {
      writer_VTK.plotParticle(p);
  }
  writer_VTK.writeFile(out_name_vtk, iteration);
}
