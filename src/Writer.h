
#ifndef _WRITER_H_
#define _WRITER_H_

#include "paratreet.decl.h"
#include <vector>

struct Writer : public CBase_Writer {
  Writer(std::string of, int n_particles);
  void receive(std::vector<Particle> particles, CkCallback cb);
  void write(CkCallback cb);

private:
  std::vector<Particle> particles;
  std::string output_file;
  int total_particles = 0;
  int expected_particles = 0;
  bool can_write = false;
  bool prev_written = false;
  int cur_dim = 0;

  void do_write();
};

Writer::Writer(std::string of, int n_particles)
  : output_file(of), total_particles(n_particles)
{
  expected_particles = n_particles / CkNumPes();
  if (expected_particles * CkNumPes() != n_particles) {
    ++expected_particles;
    if (thisIndex == CkNumPes() - 1)
      expected_particles = n_particles - thisIndex * expected_particles;
  }
}

void Writer::receive(std::vector<Particle> ps, CkCallback cb)
{
  // Accumulate received particles
  particles.insert(particles.end(), ps.begin(), ps.end());

  if (particles.size() != expected_particles) return;

  // Received expected number of particles, sort the particles
  std::sort(particles.begin(), particles.end(),
            [](const Particle& left, const Particle& right) {
              return left.order < right.order;
            });

  can_write = true;

  if (prev_written || thisIndex == 0)
    write(cb);
}

void Writer::write(CkCallback cb)
{
  prev_written = true;
  if (can_write) {
    do_write();
    cur_dim = (cur_dim + 1) % 3;
    if (thisIndex != CkNumPes() - 1) thisProxy[thisIndex + 1].write(cb);
    else if (cur_dim == 0) cb.send();
    else thisProxy[0].write(cb);
  }
}

void Writer::do_write()
{
  // Write particle accelerations to output file
  FILE *fp;
  if (thisIndex == 0 && cur_dim == 0) {
    fp = CmiFopen(output_file.c_str(), "w");
    fprintf(fp, "%d\n", total_particles);
  } else fp = CmiFopen(output_file.c_str(), "a");
  CkAssert(fp);

  for (const auto& particle : particles) {
    Real outval;
    if (cur_dim == 0) outval = particle.acceleration.x;
    else if (cur_dim == 1) outval = particle.acceleration.y;
    else if (cur_dim == 2) outval = particle.acceleration.z;
    fprintf(fp, "%.14g\n", outval);
  }

  int result = CmiFclose(fp);
  CkAssert(result == 0);
}

#endif /* _WRITER_H_ */
