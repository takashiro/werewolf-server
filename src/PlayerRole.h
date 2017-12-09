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

#pragma once

#include "common.h"

WG_NAMESPACE_BEGIN

enum class PlayerRole
{
	Unknown,

	// Team Villager
	Villager,
	Seer,
	Tamer,
	Witch,
	Hunter,
	Guard,
	Magician,
	Idiot,
	Elder,
	Knight,
	Dementor,
	Rogue,

	// Team Werewolf
	Werewolf,
	WolfKing,
	WhiteWolfKing,
	WolfBeauty,
	SecretWolf,

	// Others
	Jupiter,
	FeralChild,
	Thief,
	Bombman,

	MaxLimit
};

WG_NAMESPACE_END
