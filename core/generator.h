#ifndef PROWOGENE_CORE_GENERATOR_H_
#define PROWOGENE_CORE_GENERATOR_H_

#include "logger.h"
#include "module_interface.h"
#include "settings_interface.h"
#include "storage.h"

namespace prowogene {

/** @brief Class that allow pipeline configuration and launch.

That class allows building pipeline of modules, attaching settings, logging
process and launch pipeline with checks during runtime. */
class Generator {
 public:
    /** Delete all entities attached to generator and clear state. */
    void Clear();

    /** Attach entity for log results.
    @param [in] logger - Instance that will be used for logging. */
    void SetLogger(Logger* logger);

    /** Attach entity for data storage.
    @param [in] storage - Instance that will store transferable data
    for modules. */
    void SetStorage(Storage* storage);

    /** Add module to the end of pipeline.
    @param [in] storage - Module instance that will be pushed to the end of
                          pipeline. */
    void PushBackModule(IModule* module);

    /** Add settings for modules configuration. Order of addition is not
    significant.
    @param [in] module - Settings instance that will be added to setting
                         storage. */
    void AddSettings(ISettings* settings);

    /** Fill all already added settings instances with values from config.
    @param [in] filename - Path to JSON file that contains object with values
                           for every module filled like "settings_name" :
                           { *settings config staff* }. */
    void LoadSettings(const std::string& filename);

    /** Save all already added settings instances to file.
    @param [in] filename - Path to output JSON file.
    @param [in] pretty   - Add indentations for readability or not. Pretty
                           config is easy to read but takes up more hard drive
                           space. */
    void SaveSettings(const std::string& filename, bool pretty = true) const;

    /** Check all added settings for correctness.
    @return @c true if all settings are correct, @c false otherwise. */
    bool IsCorrect() const;

    /** Run pipeline with modules, processing them one by one in addition 
    order.
    @return @c true if generation completed successfilly, @c false if errors
            occurs during generation. */
    bool Generate();

 protected:
    /** Apply all needed settings to module.
    @param [in] module - Module that needs settings attach.
    @return @c true if all settings were added, @c false if some settings
            needed to module wasn't attached to generator. */
    virtual bool ApplySettings(IModule* module);

    /** Apply all needed data to module.
    @param [in] module - Module that needs data attach.
    @return @c true' if all data were added, @c false if some data needed to
            module wasn't attached to generator. */
    virtual bool ApplyData(IModule* module);

    /** Instance for logging. */
    Logger*                           logger_ = nullptr;
    /** Settings with access by them names. */
    std::map<std::string, ISettings*> settings_;
    /** Instance for transferable data storage. */
    Storage*                          storage_ = nullptr;
    /** Generator's modules pipeline. */
    std::list<IModule*>               modules_;
};

} // namespace prowogene

#endif // PROWOGENE_CORE_GENERATOR_H_
