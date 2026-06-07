#pragma once
#include <string>
#include "Core.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

class MapLoader
{
public:
    static bool load(std::string fileName, MapData &map)
    {
        std::ifstream file(fileName);
        if (!file.is_open())
            return false;

        json data = json::parse(file, nullptr, false);
        if (data.is_discarded())
            return false;

        MapData newMap;
        newMap.title = data["map_params"]["title"].get<std::string>();
        newMap.width = data["map_params"]["width"].get<float>();
        newMap.player.x = data["player"]["x"].get<float>();
        newMap.player.y = data["player"]["y"].get<float>();

        newMap.grounds.reserve(data["grounds"].size());
        newMap.groundEnemies.reserve(data["ground_enemies"].size());
        newMap.flyEnemies.reserve(data["fly_enemies"].size());
        newMap.powerups.reserve(data["powerups"].size());

        for (json &object : data["grounds"])
            newMap.grounds.push_back(getGroundData(object));
        for (json &object : data["ground_enemies"])
            newMap.groundEnemies.push_back(getEnemyData(object));
        for (json &object : data["fly_enemies"])
            newMap.flyEnemies.push_back(getEnemyData(object));
        for (json &object : data["powerups"])
            newMap.powerups.push_back(getPowerupData(object));

        map = std::move(newMap);
        return true;
    }
    static GroundMode getGroundMode(std::string mode)
    {
        if (mode == "horizontal")
            return GroundMode::Horizontal;
        if (mode == "vertical")
            return GroundMode::Vertical;
        if (mode == "descent")
            return GroundMode::Descent;
        if (mode == "rise")
            return GroundMode::Rise;
        return GroundMode::Static;
    }

    static AttackType getAttackType(std::string type)
    {
        if (type == "shot")
            return AttackType::Shot;
        if (type == "sword")
            return AttackType::Sword;
        return AttackType::Bubble;
    }

    static EnemyPatrolType getPatrolType(std::string type)
    {
        if (type == "wall_to_wall")
            return EnemyPatrolType::WallToWall;
        if (type == "radius")
            return EnemyPatrolType::Radius;
        if (type == "horizontal")
            return EnemyPatrolType::Horizontal;
        if (type == "vertical")
            return EnemyPatrolType::Vertical;
        return EnemyPatrolType::Fixed;
    }

    static GroundData getGroundData(json &object)
    {
        GroundData data;
        data.x = object["x"].get<float>();
        data.y = object["y"].get<float>();
        data.height = object["height"].get<float>();
        data.width = object["width"].get<float>();
        data.isBarrier = object["is_barrier"].get<bool>();
        data.mode = getGroundMode(object["mode"].get<std::string>());
        data.radius = object["radius"].get<float>();
        data.speed = object["speed"].get<float>();
        return data;
    }

    static EnemyData getEnemyData(json &object)
    {
        EnemyData data;
        data.x = object["x"].get<float>();
        data.y = object["y"].get<float>();
        data.height = object["height"].get<float>();
        data.width = object["width"].get<float>();
        data.hp = object["hp"].get<int>();
        data.speed = object["speed"].get<float>();
        data.cooldown = object["cooldown"].get<uint64_t>();
        data.attackType = getAttackType(object["attack_type"].get<std::string>());
        data.aggrRadius = object["aggr_radius"].get<float>();
        data.attackRadius = object["attack_radius"].get<float>();
        data.patrolType = getPatrolType(object["patrol_type"].get<std::string>());
        data.patrolRadius = object["patrol_radius"].get<float>();
        return data;
    }

    static PowerupData getPowerupData(json &object)
    {
        PowerupData data;
        data.x = object["x"].get<float>();
        data.y = object["y"].get<float>();
        data.height = object["height"].get<float>();
        data.width = object["width"].get<float>();
        data.attackType = getAttackType(object["attack_type"].get<std::string>());
        data.duration = object["duration"].get<uint64_t>();
        return data;
    }
};
