/**********************************************************************
 *  excludesutils.h
 **********************************************************************
 * Copyright (C) 2020-2025 MX Authors
 *
 * Authors: Adrian
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of MX Snapshot.
 *
 * MX Snapshot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MX Snapshot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MX Snapshot.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/
#pragma once

#include <QString>

#include "cmd.h"

namespace ExcludesUtils
{

// Check if the source (system-default) excludes file is newer than the user's configured excludes.
// If newer and different, populates diffOutput with unified diff text and returns true.
[[nodiscard]] bool isSourceExcludesNewer(const QString &configuredPath, const QString &sourcePath,
                                        QString &diffOutput);

// Replace the user's configured excludes file with the source default.
// Creates a timestamped backup of the existing file before replacing.
// Returns true on success.
[[nodiscard]] bool resetCustomExcludes(const QString &configuredPath, const QString &sourcePath);

// Touch the modification time of the configured excludes file to suppress future
// "newer source" prompts without actually replacing the file.
// Returns true on success.
[[nodiscard]] bool touchExcludesTimestamp(const QString &configuredPath);

// Check whether the system has an NVIDIA graphics card via glxinfo.
[[nodiscard]] bool hasNvidiaGraphicsCard(Cmd &shell);

} // namespace ExcludesUtils
