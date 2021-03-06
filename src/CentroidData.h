#ifndef PARATREET_CENTROIDDATA_H_
#define PARATREET_CENTROIDDATA_H_

#include "common.h"
#include <vector>
#include <queue>
#include "Particle.h"
#include "ParticleComp.h"
#include "OrientedBox.h"

struct CentroidData {
  Vector3D<Real> moment;
  Real sum_mass;
  Vector3D<Real> centroid; // too slow to compute this on the fly
  std::vector< std::priority_queue<Particle, std::vector<Particle>, particle_comp> > neighbors; // used for sph
  OrientedBox<Real> box;
  int count;
  Real rsq;

  CentroidData() :
  moment(Vector3D<Real> (0,0,0)), sum_mass(0), count(0), rsq(0.) {}

  CentroidData(const Particle* particles, int n_particles) : CentroidData() {
    for (int i = 0; i < n_particles; i++) {
      moment += particles[i].mass * particles[i].position;
      sum_mass += particles[i].mass;
      box.grow(particles[i].position);
    }
    centroid = moment / sum_mass;
    count += n_particles;
  }

  const CentroidData& operator+=(const CentroidData& cd) { // needed for upward traversal
    moment += cd.moment;
    sum_mass += cd.sum_mass;
    centroid = moment / sum_mass;
    box.grow(cd.box);
    Vector3D<Real> delta1 = centroid - box.lesser_corner;
    Vector3D<Real> delta2 = box.greater_corner - centroid;
    delta1.x = (delta1.x > delta2.x ? delta1.x : delta2.x);
    delta1.y = (delta1.y > delta2.y ? delta1.y : delta2.y);
    delta1.z = (delta1.z > delta2.z ? delta1.z : delta2.z);
    rsq = delta1.lengthSquared();

    count += cd.count;
    return *this;
  }

  const CentroidData& operator= (const CentroidData& cd) {
    moment = cd.moment;
    sum_mass = cd.sum_mass;
    centroid = cd.centroid;
    box = cd.box;
    count = cd.count;
    rsq = cd.rsq;
    return *this;
  }

  void pup(PUP::er& p) {
    p | moment;
    p | sum_mass;
    p | centroid;
    p | box;
    p | count;
    p | rsq;
  }

};

#endif // PARATREET_CENTROID_H_
