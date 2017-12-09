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

#include <User.h>

#include "PlayerRole.h"

WG_NAMESPACE_BEGIN

class WerewolfDriver;

class Player
{
public:
	using Role = PlayerRole;

	Player(KA_IMPORT User *user);
	~Player();

	KA_IMPORT uint uid() const;

	Role role() const;
	void setRole(Role role);

private:
	KA_DECLARE_PRIVATE
};

WG_NAMESPACE_END