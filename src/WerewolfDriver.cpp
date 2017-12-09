/***********************************************************************
Werewolf Game Assistant
Copyright (C) 2017  Kazuichi Takashiro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

takashiro@qq.com
************************************************************************/

#include "WerewolfDriver.h"

#include "cmd.h"
#include "Player.h"
#include "PlayerRole.h"

#include <Room.h>
#include <Semaphore.h>
#include <Json.h>

#include <vector>
#include <thread>
#include <algorithm>
#include <random>

KA_USING_NAMESPACE

WG_NAMESPACE_BEGIN

struct WerewolfDriver::Private
{
	Json config;
	std::vector<PlayerRole> roles;
	std::vector<Player *> players;

	uint next_role_index;
	Semaphore fetched;

	void updateConfig()
	{
		JsonObject config;

		JsonArray roles;
		for (PlayerRole role : this->roles) {
			roles.push_back(static_cast<int>(role));
		}
		config["roles"] = roles;

		this->config = std::move(config);
	}
};

WerewolfDriver::WerewolfDriver()
	: d(new Private)
{
}

WerewolfDriver::~WerewolfDriver()
{
	for (Player *player : d->players) {
		delete player;
	}
	delete d;
}

void WerewolfDriver::setConfig(const KA_IMPORT Json &config)
{
	if (!config.isObject()) {
		return;
	}

	JsonObject setting = config.toObject();
	if (setting.find("roles") != setting.end()) {
		Json role_list = setting.at("roles");
		if (role_list.isArray()) {
			JsonArray roles = role_list.toArray();
			d->roles.clear();
			int max_value = static_cast<int>(PlayerRole::MaxLimit);
			for (const Json &role : roles) {
				int role_value = role.toInt();
				if (role_value < max_value) {
					d->roles.push_back(static_cast<PlayerRole>(role_value));
				}
			}
		}
	}

	d->updateConfig();
}

const Json &WerewolfDriver::config() const
{
	return d->config;
}

void WerewolfDriver::run()
{
	// Show cards to God
	JsonArray roles;
	roles.reserve(d->roles.size());
	for (PlayerRole role : d->roles) {
		roles.push_back(static_cast<int>(role));
	}
	User *god = room()->owner();
	god->notify(cmd::ArrangeRole, roles);

	// Shuffle cards
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(d->roles.begin(), d->roles.end(), g);
	d->next_role_index = 0;

	// Wait for at most 10 minutes
	uint role_num = static_cast<uint>(d->roles.size());
	d->fetched.acquire(role_num, 600);
}

void WerewolfDriver::end()
{
}

void WerewolfDriver::addPlayer(KA_IMPORT User *user)
{
	if (user->data()) {
		return;
	}

	Player *player = new Player(user);
	user->setData(player);
	d->players.push_back(player);
}

void WerewolfDriver::removePlayer(KA_IMPORT User *user)
{
	Player *player = dynamic_cast<Player *>(user->data());
	if (player == nullptr) {
		return;
	}

	auto iter = std::find(d->players.begin(), d->players.end(), player);
	if (iter != d->players.end()) {
		d->players.erase(iter);
		user->setData(nullptr);
		delete player;
	}
}

void WerewolfDriver::setRoles(std::vector<PlayerRole> &&roles)
{
	d->roles = std::move(roles);
}

const std::vector<PlayerRole> &WerewolfDriver::roles() const
{
	return d->roles;
}

const std::map<int, KA_IMPORT UserAction> *WerewolfDriver::actions() const
{
	static std::map<int, UserAction> action_map = CreateWerewolfActions();
	return &action_map;
}

PlayerRole WerewolfDriver::fetchRole(Player *player)
{
	PlayerRole role = d->next_role_index < d->roles.size() ? d->roles.at(d->next_role_index) : PlayerRole::Unknown;
	d->next_role_index++;
	d->fetched.release(1);
	return role;
}

WG_NAMESPACE_END
