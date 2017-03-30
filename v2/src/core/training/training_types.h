//
// Created by Arseny Tolmachev on 2017/03/30.
//

#ifndef JUMANPP_TRAINING_TYPES_H
#define JUMANPP_TRAINING_TYPES_H

namespace jumanpp {
namespace core {
namespace training {

// values of this enum specify how
// the difference between top and gold
// features will be computed
enum class TrainingMode {
  // compute features for the whole example
  Full,
  // compute features until gold falls off the beam
  FalloffBeam,
  // compute features until maximum violation -
  // difference between top1 and gold score.
  MaxViolation
};

struct ScwConfig {
  float C = 1.0f;
  float phi = 1.0f;
};

struct TrainingConfig {
  TrainingMode mode = TrainingMode::Full;
  u32 numHashedFeatures;
  ScwConfig scw;
  u32 randomSeed = 0xdeadbeef;
};

}  // training
}  // core
}  // jumanpp

#endif  // JUMANPP_TRAINING_TYPES_H
