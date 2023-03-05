/* common.h -- common types and marco definitions

   Copyright (C) 2022-2023 OSN Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef __COMMON_H__
#define __COMMON_H__

#include "../config.h"

#if !defined(__GNUC__) && !defined(__clang__)
#error Only GNU C compiler and clang is supported.
#endif

#define LIGHTBOT_GIT_REPO PACKAGE_URL
#define LIGHTBOT_AUTHOR_NAME "Ar Rakin"
#define LIGHTBOT_CONFIG_DIR "lightbot"

#endif /* __COMMON_H__ */