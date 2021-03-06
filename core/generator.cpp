#include "generator.h"

#include <chrono>
#include <fstream>

namespace prowogene {

using std::list;
using std::string;
using utils::Array2D;
using utils::JsonValue;
using utils::JsonObject;
using utils::JsonType;
using utils::InputString;

void Generator::Clear() {
    for (auto& settings : settings_) {
        if (settings.second) {
            delete settings.second;
        }
    }
    settings_.clear();

    for (auto& module : modules_) {
        if (module) {
            delete module;
        }
    }
    modules_.clear();

    if (storage_) {
        delete storage_;
        storage_ = nullptr;
    }

    if (logger_) {
        delete logger_;
        logger_ = nullptr;
    }
}

void Generator::SetLogger(Logger* logger) {
    logger_ = logger;
}

void Generator::SetStorage(prowogene::Storage* storage) {
    storage_ = storage;
}

void Generator::PushBackModule(IModule* module) {
    modules_.push_back(module);
}

void Generator::AddSettings(ISettings* settings) {
    settings_[settings->GetName()] = settings;
}

void Generator::LoadSettings(const string& filename) {
    JsonValue json_file;
    json_file.Parse(filename, InputString::FILENAME);
    if (json_file.GetType() != JsonType::OBJECT) {
        return;
    }

    JsonObject config = json_file;
    for (auto& sub_config : config) {
        auto founded = settings_.find(sub_config.first);
        if (founded != settings_.end()) {
            founded->second->Deserialize(sub_config.second);
        }
    }
}

void Generator::SaveSettings(const string& filename, bool pretty) const {
    JsonObject config;
    for (auto& settings : settings_) {
        config[settings.first] = settings.second->Serialize();
    }
    JsonValue json_file = config;
    string str = json_file.ToString();
    std::ofstream out_file(filename);
    if (out_file.is_open()) {
        out_file.write(str.c_str(), str.size());
    }
}

bool Generator::IsCorrect() const {
    for (const auto& s : settings_) {
        if (!s.second->IsCorrect()) {
            const string msg = "Incorrect settings : '" + s.first + "'.";
            logger_->LogError(nullptr, msg);
            return false;
        }
    }
    return true;
}

bool Generator::Generate() {
    if (logger_) {
        logger_->DrawPipeline(modules_);
    }

    if (!IsCorrect()) {
        return false;
    }

    logger_->LogMessage("Generation started.");
    auto time_beg = std::chrono::high_resolution_clock::now();

    for (auto& module : modules_) {
        if (!module) {
            logger_->LogError(module, "Module didn't found.");
            return false;
        }

        if (logger_) {
            logger_->ModuleStarted(module);
        }

        if (!ApplySettings(module)) {
            return false;
        }
        module->Init();
        if (!ApplyData(module)) {
            return false;
        }
        bool success = module->Process();
        if (!success) {
            if (logger_) {
                logger_->LogError(module);
            }
            module->Deinit();
            return false;
        }
        module->Deinit();

        if (logger_) {
            logger_->ModuleEnded(module);
        }
    }

    logger_->LogMessage("Generation ended.");
    auto time_end = std::chrono::high_resolution_clock::now();
    double seconds = (time_end - time_beg).count() / 1000000000.0;
    logger_->LogMessage("Time elapsed: " + std::to_string(seconds) + " sec.");
    return true;
}

bool Generator::ApplySettings(IModule* module) {
    const list<string> settings_keys = module->GetNeededSettings();
    for (const auto& i : settings_keys) {
        const auto needed = settings_.find(i);
        if (needed == settings_.end()) {
            const string msg = "No settings with key '" + i + "'.";
            logger_->LogError(module, msg);
            return false;
        }
        module->ApplySettings(needed->second);
    }
    return true;
}

bool Generator::ApplyData(IModule* module) {
    const list<string> data_keys = module->GetNeededData();
    for (const string& key : data_keys) {
        if (!storage_->GetData<void*>(key)) {
            if (logger_) {
                const string msg = "No data with key '" + key + "'.";
                logger_->LogError(module, msg);
            }
            return false;
        }
    }
    module->SetStorage(storage_);
    return true;
}

} // namespace prowogene
