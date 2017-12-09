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

#include "Player.h"

#include "cmd.h"

KA_USING_NAMESPACE

WG_NAMESPACE_BEGIN

struct Player::Private
{
	uint uid;
	PlayerRole role;

	Private()
		: uid(0)
		, role(PlayerRole::Unknown)
	{
	}
};

Player::Player(KA_IMPORT User *user)
	: d(new Private)
{
	d->uid = user->id();
}

Player::~Player()
{
	delete d;
}

KA_IMPORT uint Player::uid() const
{
	return d->uid;
}

Player::Role Player::role() const
{
	return d->role;
}

void Player::setRole(Role role)
{
	d->role = role;
}

WG_NAMESPACE_END
