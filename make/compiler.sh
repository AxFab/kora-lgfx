#!/bin/bash
#      This file is part of the KoraOS project.
#  Copyright (C) 2015-2021  <Fabien Bavent>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

TMP=`mktemp XXXXXX.c`
cat > "$TMP" << EOF
#if $1
y
#else
n
#endif
EOF
shift
CC="$1"
shift
"$CC" -E "$TMP" "$@" | grep -v -e '^#' -e '^\s*$'
rm "$TMP"
