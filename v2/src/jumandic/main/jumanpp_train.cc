//
// Created by Arseny Tolmachev on 2017/05/09.
//

#include "jumanpp_train.h"
#include "args.h"
#include "core/training/training_env.h"
#include "jumandic/shared/jumandic_env.h"
#include "util/logging.hpp"

#include <iostream>

using namespace jumanpp;
namespace t = jumanpp::core::training;

Status parseArgs(int argc, const char** argv, t::TrainingArguments* args) {
  args::ArgumentParser parser{"Juman++ Training"};

  args::Group ioGroup{parser, "Input/Output"};
  args::ValueFlag<std::string> modelFile{ioGroup,
                                         "modelInFile",
                                         "Filename of preprocessed dictionary",
                                         {"model-input"}};
  args::ValueFlag<std::string> modelOutput{ioGroup,
                                           "modelOutFile",
                                           "Model will be written to this file",
                                           {"model-output"}};
  args::ValueFlag<std::string> corpusFile{
      ioGroup,
      "corpus",
      "Filename of corpus that will be used for training",
      {"corpus"}};

  args::Group trainingParams{parser, "Training parameters"};
  args::ValueFlag<u32> paramSizeExponent{
      trainingParams,
      "SIZE",
      "Param size will be 2^SIZE, 15 (32k) default",
      {"size"},
      15};
  args::ValueFlag<u32> randomSeed{trainingParams,
                                  "SEED",
                                  "RNG seed, 0xdeadbeef default",
                                  {"seed"},
                                  0xdeadbeefU};
  args::ValueFlag<u32> beamSize{
      trainingParams, "BEAM", "Beam size, 5 default", {"beam"}, 5};
  args::ValueFlag<u32> batchSize{
      trainingParams, "BATCH", "Batch Size, 1 default", {"batch"}, 1};
  args::ValueFlag<u32> numThreads{
      trainingParams, "THREADS", "# of threads, 1 default", {"threads"}, 1};
  args::ValueFlag<u32> maxBatchIters{trainingParams,
                                     "BATCH_ITERS",
                                     "max # of batch iterations",
                                     {"max-batch-iters"},
                                     1};
  args::ValueFlag<u32> maxEpochs{
      trainingParams, "EPOCHS", "max # of epochs", {"max-epochs"}, 1};
  args::ValueFlag<float> epsilon{
      trainingParams, "EPSILON", "stopping epsilon", {"epsilon"}, 1e-3f};

  try {
    parser.ParseCLI(argc, argv);
  } catch (args::ParseError& e) {
    std::cout << e.what() << "\n" << parser;
    std::exit(1);
  }

  args->trainingConfig.beamSize = beamSize.Get();
  args->batchSize = batchSize.Get();
  args->numThreads = numThreads.Get();
  args->modelFilename = modelFile.Get();
  args->outputFilename = modelOutput.Get();
  args->corpusFilename = corpusFile.Get();
  auto sizeExp = paramSizeExponent.Get();
  args->trainingConfig.featureNumberExponent = sizeExp;
  args->trainingConfig.randomSeed = randomSeed.Get();
  args->batchMaxIterations = maxBatchIters.Get();
  args->maxEpochs = maxEpochs.Get();
  args->batchLossEpsilon = epsilon.Get();

  if (sizeExp > 31) {
    return Status::InvalidState() << "size exponent was too large: " << sizeExp
                                  << ", maximum allowed is 31";
  }

  return Status::Ok();
}

void doTrain(core::training::TrainingEnv& env,
             const t::TrainingArguments& args) {
  float lastLoss = 0.0f;
  for (int nepoch = 0; nepoch < args.maxEpochs; ++nepoch) {
    env.resetInput();
    Status s = env.trainOneEpoch();
    if (!s) {
      LOG_ERROR() << "failed to train: " << s.message;
      exit(1);
    }

    LOG_INFO() << "EPOCH #" << nepoch << " finished, loss=" << env.epochLoss();

    if (std::abs(lastLoss - env.epochLoss()) < args.batchLossEpsilon) {
      return;
    }
  }
}

int main(int argc, const char** argv) {
  t::TrainingArguments args{};
  Status s = parseArgs(argc, argv, &args);
  if (!s) {
    LOG_ERROR() << "failed to parse arguments: " << s.message;
    return 1;
  }

  core::JumanppEnv env;

  s = env.loadModel(args.modelFilename);
  if (!s) {
    LOG_ERROR() << "failed to read model from disk: " << s.message;
    return 1;
  }
  env.setBeamSize(args.trainingConfig.beamSize);

  t::TrainingEnv exec{args, &env};

  s = exec.initFeatures(nullptr);

  if (!s) {
    LOG_ERROR() << "failed to initialize features: " << s.message;
    return 1;
  }

  s = exec.initOther();

  if (!s) {
    LOG_ERROR() << "failed to initialize training process: " << s.message;
    return 1;
  }

  s = exec.loadInput(args.corpusFilename);
  if (!s) {
    LOG_ERROR() << "failed to open corpus filename: " << s.message;
    return 1;
  }

  doTrain(exec, args);

  auto model = env.modelInfoCopy();
  exec.exportScwParams(&model);

  core::model::ModelSaver saver;
  s = saver.open(args.outputFilename);
  if (!s) {
    LOG_ERROR() << "failed to open file [" << args.outputFilename
                << "] for saving model: " << s.message;
    return 1;
  }

  s = saver.save(model);
  if (!s) {
    LOG_ERROR() << "failed to save model: " << s.message;
  }

  return 0;
}