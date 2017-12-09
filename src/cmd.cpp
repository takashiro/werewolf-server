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

#include "cmd.h"

#include "Player.h"
#include "WerewolfDriver.h"

#include <Room.h>
#include <User.h>
#include <UserAction.h>
#include <Json.h>

#include <map>

KA_USING_NAMESPACE

WG_NAMESPACE_BEGIN

std::map<int, KA_IMPORT UserAction> CreateWerewolfActions()
{
	static std::map<int, UserAction> actions;

	actions[cmd::FetchRole] = [] (User *user, const Json &) {
		Room *room = user->room();
		if (room == nullptr) {
			return;
		}

		WerewolfDriver *driver = dynamic_cast<WerewolfDriver *>(room->driver());
		if (driver == nullptr) {
			return;
		}

		Player *player = dynamic_cast<Player *>(user->data());
		if (player == nullptr) {
			return;
		}

		PlayerRole role = driver->fetchRole(player);
		user->notify(cmd::UpdateRole, static_cast<int>(role));
	};

	return actions;
}

WG_NAMESPACE_END
