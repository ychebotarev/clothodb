#pragma once

#include <memory>
#include <map>
#include <functional>
#include "command_type.h"

////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////
namespace cdb { namespace db {
    class idatabase;
    class ilookup;
}}
///////////////////////////////////////////


namespace cdb {
namespace svr {

class command_replay
{
public:
    command_replay(const char* buffer);
    ~command_replay();

    void replay(
        std::shared_ptr<cdb::db::idatabase> database,
        std::shared_ptr<cdb::db::ilookup> lookup);
private:
    command_type get_command();

    void replay_create_namespace();
    void replay_delete_namespace();
    void replay_create_timeseries();
    void replay_delete_timeseries();
    void replay_add_value();

private:
    const char* m_buffer;
    size_t m_position;
    std::shared_ptr<cdb::db::idatabase> m_database;
    std::shared_ptr<cdb::db::ilookup> m_lookup;
private:
    static std::map<command_type, void(command_replay::*)()> sCommands;
};

}}

