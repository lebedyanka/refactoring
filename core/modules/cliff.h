#ifndef PROWOGENE_CORE_MODULES_CLIFF_H_
#define PROWOGENE_CORE_MODULES_CLIFF_H_

#include "basis.h"
#include "general_settings.h"
#include "system_settings.h"
#include "utils/array2d.h"

namespace prowogene {
namespace modules {

/** Cliff settings config name. */
const std::string kConfigCliff = "cliff";

/** @brief Cliff module settings. */
struct CliffSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Module enabled. */
    bool  enabled = false;
    /** Cliff details size. Less values means bigger details. [1, ...]. */
    int   octaves = 3;
    /** Count of cliff levels. [1, ...] */
    int   levels = 8;
    /** Random number generation seed. Any. */
    int   seed = 0;
    /** Random noise distortion size. [0.0, 1.0]. */
    float grain = 0.0325f;
};


/** @brief Module for cliffs creation.

Creates cliffs at some levels and adds them to height map. */
class CliffModule : public IModule {
 public:
    /** @copydoc IModule::SetStorage */
    void SetStorage(Storage* storage) override;
    /** @copydoc IModule::Process */
    bool Process() override;
    /** @copydoc IModule::GetNeededData */
    std::list<std::string> GetNeededData() const override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Height map from data storage. */
    utils::Array2D<float>* height_map_ = nullptr;
    /** Settings for module. */
    struct {
        /** Basis settings. */
        BasisSettings   basis;
        /** Cliff settings. */
        CliffSettings   cliff;
        /** General settings. */
        GeneralSettings general;
        /** System settings. */
        SystemSettings  system;
    } settings_;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_CLIFF_H_
