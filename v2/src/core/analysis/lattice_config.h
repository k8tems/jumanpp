//
// Created by Arseny Tolmachev on 2017/03/02.
//

#ifndef JUMANPP_LATTICE_CONFIG_H
#define JUMANPP_LATTICE_CONFIG_H

#include "util/types.hpp"

namespace jumanpp {
namespace core {
namespace analysis {

class LatticePlugin;

using LatticePosition = u16;
using Score = float;

struct alignas(alignof(u32)) LatticeNodePtr {
  u16 boundary;
  u16 position;
};

inline bool operator==(const LatticeNodePtr& p1, const LatticeNodePtr& p2) {
  return p1.boundary == p2.boundary && p1.position == p2.position;
}

struct alignas(alignof(u64)) ConnectionPtr {
  // boundary where the connection was created
  u16 boundary;
  // offset of ending node inside the boundary (get the actual pointer from
  // boundary.ends[left])
  u16 left;
  // offset of starting node inside the boundary
  u16 right;
  // beam index for getting additional context
  // the context is stored in the current boundary
  u16 beam;

  // pointer to the previous entry
  const ConnectionPtr* previous;

  LatticeNodePtr latticeNodePtr() const {
    return LatticeNodePtr{boundary, right};
  }
};

inline bool operator==(const ConnectionPtr& p1, const ConnectionPtr& p2) {
  return p1.boundary == p2.boundary && p1.left == p2.left &&
         p1.right == p2.right && p1.beam == p2.beam &&
         p1.previous == p2.previous;
}

struct LatticeConfig {
  u32 entrySize;
  u32 numPrimitiveFeatures;
  u32 numFeaturePatterns;
  u32 numFinalFeatures;
  u32 beamSize;
  u32 scoreCnt;
};

struct LatticeBoundaryConfig {
  u32 boundary;
  u32 endNodes;
  u32 beginNodes;
};

struct ConnectionBeamElement {
  ConnectionPtr ptr;
  Score totalScore;
};

class Lattice;

}  // namespace analysis
}  // namespace core
}  // namespace jumanpp

#endif  // JUMANPP_LATTICE_CONFIG_H
