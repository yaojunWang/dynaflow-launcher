//
// Copyright (c) 2020, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0

#include "Configuration.h"
#include "Context.h"
#include "Dico.h"
#include "Log.h"
#include "Message.hpp"
#include "Options.h"
#include "version.h"

#include <DYNError.h>
#include <DYNIoDico.h>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <sstream>

static const char* dictPrefix = "DFLMessages_";

static std::string
getMandatoryEnvVar(const std::string& key) {
  char* var = getenv(key.c_str());
  if (var != NULL) {
    return std::string(var);
  } else {
    // we cannot use dictionnary errors since they may not be initialized yet
    LOG(error) << "Cannot find environnement variable " << key << " : please check runtime environement" << LOG_ENDL;
    std::exit(EXIT_FAILURE);
  }
}

static void
initializeDynawo(const std::string& locale) {
  boost::shared_ptr<DYN::IoDicos> dicos = DYN::IoDicos::getInstance();
  dicos->addPath(getMandatoryEnvVar("DYNAWO_RESOURCES_DIR"));
  dicos->addDico("ERROR", "DYNError", locale);
  dicos->addDico("TIMELINE", "DYNTimeline", locale);
  dicos->addDico("CONSTRAINT", "DYNConstraint", locale);
  dicos->addDico("LOG", "DYNLog", locale);
}

int
main(int argc, char* argv[]) {
  try {
    DYN::Trace::init();
    dfl::common::Options options;

    auto parsing_status = options.parse(argc, argv);

    if (!std::get<0>(parsing_status) || std::get<1>(parsing_status) == dfl::common::Options::Request::HELP) {
      LOG(info) << options << LOG_ENDL;
      return EXIT_SUCCESS;
    }
    if (std::get<1>(parsing_status) == dfl::common::Options::Request::VERSION) {
      LOG(info) << DYNAFLOW_LAUNCHER_VERSION_STRING << LOG_ENDL;
      return EXIT_SUCCESS;
    }

    auto& runtimeConfig = options.config();
    dfl::inputs::Configuration config(runtimeConfig.configPath);
    dfl::common::Log::init(options, config.outputDir(), false);

    std::string res = getMandatoryEnvVar("DYNAWO_RESOURCES_DIR");
    std::string root = getMandatoryEnvVar("DYNAFLOW_LAUNCHER_INSTALL");
    std::string locale = getMandatoryEnvVar("DYNAFLOW_LAUNCHER_LOCALE");
    boost::filesystem::path dictPath(root);
    dictPath.append("etc");
    std::string dict = dictPrefix + locale + ".dic";
    dictPath.append(dict);
    dfl::common::Dico::configure(dictPath.c_str());
    initializeDynawo(locale);

    if (!boost::filesystem::is_regular_file(dictPath)) {
      // we cannot use dictionnary errors since they are not be initialized yet
      LOG(error) << "Dictionary " << dictPath << " not found: check runtime environment" << LOG_ENDL;
      return EXIT_FAILURE;
    }
    if (!boost::filesystem::exists(boost::filesystem::path(runtimeConfig.networkFilePath))) {
      LOG(error) << MESS(NetworkFileNotFound, runtimeConfig.networkFilePath) << LOG_ENDL;
      return EXIT_FAILURE;
    }
    LOG(info) << MESS(InputsInfo, runtimeConfig.networkFilePath, runtimeConfig.configPath) << LOG_ENDL;

    boost::filesystem::path parFilesDir(root);
    parFilesDir.append("etc");

    dfl::Context::ContextDef def{runtimeConfig.networkFilePath, runtimeConfig.dynawoLogLevel, parFilesDir.generic_string(), res, locale};
    dfl::Context context(def, config);

    if (!context.process()) {
      return EXIT_FAILURE;
    }

    context.exportOutputs();
    context.execute();

    dfl::common::Log::init(options, config.outputDir(), true);

    LOG(info) << MESS(SimulationEnded, context.basename()) << LOG_ENDL;

    return EXIT_SUCCESS;
  } catch (DYN::Error& e) {
    std::cerr << "Dynawo: " << e.what() << std::endl;
    LOG(error) << "Dynawo: " << e.what() << LOG_ENDL;
    return EXIT_FAILURE;
  } catch (DYN::MessageError& e) {
    std::cerr << "Dynawo: " << e.what() << std::endl;
    LOG(error) << "Dynawo: " << e.what() << LOG_ENDL;
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    LOG(error) << e.what() << LOG_ENDL;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error" << std::endl;
    LOG(error) << "Unknown error" << LOG_ENDL;
    return EXIT_FAILURE;
  }
}
