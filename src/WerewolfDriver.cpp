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
	uint salt;

	bool ready;
	std::vector<PlayerRole> cards;
	Semaphore fetched;

	Private()
		: ready(false)
	{
		std::random_device rd;
		std::mt19937 g(rd());
		std::uniform_int_distribution<uint> dist(0, 0xFFFFFFFFu);
		salt = dist(g);
	}

	void updateConfig()
	{
		JsonObject config;
		config["salt"] = salt;

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
	// Shuffle cards
	std::random_device rd;
	std::mt19937 g(rd());
	d->cards = d->roles;
	std::shuffle(d->cards.begin(), d->cards.end(), g);

	// Wait for at most 10 minutes
	d->ready = true;
	while (d->fetched.acquire(1, 600)) {
		if (d->cards.empty()) {
			break;
		}
	}
	d->ready = false;
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

PlayerRole WerewolfDriver::fetchRole()
{
	if (!d->ready || d->cards.empty()) {
		return PlayerRole::Unknown;
	}

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<uint> dist(0, static_cast<uint>(d->cards.size()) - 1);
	uint chosen_id = dist(g);

	if (chosen_id < d->cards.size()) {
		PlayerRole role = d->cards.at(chosen_id);
		d->cards.erase(d->cards.begin() + chosen_id);
		d->fetched.release(1);
		return role;
	} else {
		return PlayerRole::Unknown;
	}
}

WG_NAMESPACE_END
