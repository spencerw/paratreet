#ifndef SIMPLE_MULTIDATA_H_
#define SIMPLE_MULTIDATA_H_

#include "Particle.h"
#include "Node.h"
#include "common.h"
#include "simple.decl.h"

template <typename Data>
struct MultiData {
  int n_particles;
  int n_nodes;
  Particle particles [BRANCH_FACTOR * BRANCH_FACTOR * MAX_PARTICLES_PER_LEAF];
  Node<Data> nodes [1 + BRANCH_FACTOR + BRANCH_FACTOR * BRANCH_FACTOR];
  MultiData();
  MultiData(Particle*, int, Node<Data>*, int);
  void pup(PUP::er& p);
};

template <typename Data>
inline MultiData<Data>::MultiData() {
  n_particles = 0;
  n_nodes = 0;  
}

template <typename Data>
inline MultiData<Data>::MultiData(Particle* particlesi, int n_particlesi, Node<Data>* nodesi, int n_nodesi) {
  n_particles = n_particlesi;
  n_nodes = n_nodesi;
  memcpy(particles, particlesi, n_particles * sizeof(Particle));
  memcpy(nodes, nodesi, n_nodes * sizeof(Node<Data>));
}

template <typename Data>
void MultiData<Data>::pup(PUP::er& p) {
  p | n_particles;
  p | n_nodes;
  PUParray(p, particles, BRANCH_FACTOR * BRANCH_FACTOR * MAX_PARTICLES_PER_LEAF);
  PUParray(p, nodes, 1 + BRANCH_FACTOR + BRANCH_FACTOR * BRANCH_FACTOR);
}


#endif // SIMPLE_MULTIDATA_H_