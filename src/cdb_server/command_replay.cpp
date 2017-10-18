#include "command_replay.h"

#include "src/cdb_database/idatabase.h"
#include "src/cdb_database/lookup.h"

namespace cdb {
namespace svr {

std::map<command_type, void(command_replay::*)()> command_replay::sCommands =
{
    { command_type::create_namespace, &command_replay::replay_create_namespace },
    { command_type::delete_namespace, &command_replay::replay_delete_namespace },
    { command_type::create_timeseries, &command_replay::replay_create_timeseries },
    { command_type::delete_timeseries, &command_replay::replay_delete_timeseries },
    { command_type::add_value, &command_replay::replay_add_value }

};
command_replay::command_replay(const char* buffer)
{
    m_buffer = buffer;

}

command_replay::~command_replay()
{
    delete[] m_buffer;
}

void command_replay::replay(
    std::shared_ptr<cdb::db::idatabase> database,
    std::shared_ptr<cdb::db::ilookup> lookup)
{
    m_database = database;
    m_lookup = lookup;
    m_position = 0;

    auto command = get_command();
    auto replay_iter = sCommands.find(command);
    if (replay_iter == sCommands.end()) return;
    
    auto replay_function = replay_iter->second;
    (this->*replay_function)();
}

command_type command_replay::get_command()
{
    return (command_type)m_buffer[m_position++];
}

void command_replay::replay_create_namespace()
{

}
void command_replay::replay_delete_namespace()
{

}
void command_replay::replay_create_timeseries()
{

}

void command_replay::replay_delete_timeseries()
{

}

void command_replay::replay_add_value()
{
    uint32_t* puid = (uint32_t*)&m_buffer[m_position];
    m_position += sizeof(uint32_t);
    uint64_t* pvalue = (uint64_t*)&m_buffer[m_position];
    m_position += sizeof(uint64_t);
    uint64_t* ptimestamp = (uint64_t*)&m_buffer[m_position];
    m_position += sizeof(uint64_t);

    m_database->add_value(
        *puid,
        *pvalue,
        *ptimestamp);
}


}}
