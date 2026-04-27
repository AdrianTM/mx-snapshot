# Debian / Arch Unification Plan

Plan for collapsing the long-running `main` (Debian/MX) and `arch` branches into a
single source tree that builds for both distros. Drafted 2026-04-26.

**Baseline:** `main` is the integration target. Merge base with `arch` is
`df27c9b` (2025-12-20); 65 commits ahead on `main`, 73 ahead on `arch`.

**Strategy in one paragraph:** keep one repo and one `main` branch. Use
`Settings::isArch` (runtime, set from `/etc/os-release`) as the primary gate,
with `ARCH_BUILD` (compile-time CMake option) as a secondary gate for the
handful of sites that must skip Debian-only checks (squashfs config probe,
`/etc/localtime` exclusion). Packaging differences live in `debian/` vs
`PKGBUILD` + `build.sh --arch`. Ship two binaries; runtime detection is for
robustness, not cross-distro execution from a single binary.

> **Step 2 is deferred.** The `BindRootManager` Qt port of `installed-to-live`
> is the riskiest change in this plan and is not required for unification.
> Bring its source into the tree but gate its use on `isArch` so Debian
> continues to call `installed-to-live` verbatim. Plan to land it as a
> standalone change later, after Arch users have shaken out bugs.

## Step 0 — Prep

- [x] Create `unify` branch off `main`.
- [x] Tag both tips for archival: `pre-unify-main`, `pre-unify-arch`.
- [x] List the duplicate-rewrite commits that exist on both branches with
      slightly different code (do **not** cherry-pick these from `arch` —
      `main`'s versions win because they are newer):
  - C++ helper binary replacing shell helper (`2d58559` main / `2f29edf` arch)
  - Show help and license in app window (`b520f0c` / `875a540`)
  - Use pkexec for snapshot-lib actions
  - Validate snapshot-lib app name
  - CLI/GUI: backup exclusion file on reset
  - polkit: handle gui/cli logs and configs
  - GUI: keep boot options after NVIDIA prompt
  - Fix exclude size calc on sparse files
  - Avoid symlink glob dereference in excludes
  - Quote exclude filename
  - Improve space calculation for separate /home partitions
  - Fix exclude size double counting
  - Adjust space validation timing for GUI builds
  - Fix GUI/CLI space checks
  - Accept any case response
  - Fix check, the translated option might conflict
  - Remove exclusion `/var/lib/sddm/`
  - Fix size estimation cleanup
- [ ] Verify each item above is functionally equivalent on both branches; if
      `arch` has a meaningful improvement, port that delta by hand on top of
      `main`'s version.

## Step 1 — Distro detection scaffolding

Bring the smallest amount of `arch` content needed for everything else to
compile and detect distro at runtime.

- [x] `CMakeLists.txt`: add `option(ARCH_BUILD "Build for Arch Linux" OFF)` and
      `add_compile_definitions(ARCH_BUILD)` when set.
- [x] `CMakeLists.txt`: add `find_library(LIBCRYPT crypt)` and link
      conditionally to both targets.
- [x] `CMakeLists.txt`: layer arch's version-detection fallback chain on top of
      `main`'s `dpkg-parsechangelog` logic — `git describe --tags --abbrev=0
      --match "[0-9]*"` → `dpkg-parsechangelog` → regex match on
      `debian/changelog` first line. So git checkouts, source tarballs, and
      Arch builds all yield a version.
- [x] `Settings`: add `bool isArch{}` member.
- [x] `Settings`: add helpers `readOsReleaseValue`, `trimQuotesValue`,
      `ensureCowSpacesize`.
- [x] `Settings::setVariables`: keep `main`'s direct-read `lsb-release` path,
      fall through to `os-release` when absent, set `isArch` from
      `ARCH_BUILD` define + os-release `NAME` containing "Arch", and add the
      MX-on-Arch hybrid detection (`distroVersion.contains("Arch")` →
      `projectName += parts[0]`, `codename = parts[1] + " " + parts[0]`).
- [x] `Settings::setVariables`: branch `fullDistroName` format (Arch:
      `<distroVersion>_<arch>`; MX: `<projectName>-<distroVersion>_<arch>`).
- [x] `Settings::setVariables`: call `ensureCowSpacesize` on `bootOptions`
      (also in `setMonthlySnapshot`).
- [x] `Settings::getDebianVerNum`: return `Bookworm` when `/etc/debian_version`
      is missing (Arch case) instead of crashing.
- [x] `Settings::checkCompression`: short-circuit return `true` when `isArch`.
- [x] `Settings::checkConfiguration`: skip squashfs probe when `isArch`.
- [x] `Settings::selectKernel`: skip squashfs probe when `isArch`.
- [x] `main.cpp::checkSquashfs`: early return when `ARCH_BUILD` is defined.
- [x] `Settings::otherExclusions`: wrap the `/etc/localtime` exclusion logic in
      `#ifndef ARCH_BUILD`.
- [x] Smoke-test on MX: verify `isArch=false`, no behavior change.
- [x] Smoke-test build with `-DARCH_BUILD=ON`: verify it compiles.

## Step 2 — `BindRootManager` port (DEFERRED)

> **Not part of this unification pass.** Bring the source in (Step 3 needs
> nothing from it), but Debian keeps invoking `installed-to-live` directly.
> Track here so it isn't forgotten.

- [ ] Cherry-pick `e16c9a4 Ported installed-to-live to Qt code`,
      `75bfd7e Move bind-root cleanup into snapshot-lib`,
      `6926bf8 Simplify bind-root cleanup`.
- [ ] Add `cleanup_bindrootmanager` action to `polkit/snapshot-lib`.
- [ ] In `Work::setupEnv`, `Work::cleanUp`, `Work::checkAndMoveWorkDir`,
      `Work::createIso`, and `Settings` ctor: gate every `BindRootManager` use
      with `if (settings->isArch)` (runtime) or `#ifdef ARCH_BUILD` (compile
      time, in the early-ctor cleanup site where `Settings` isn't populated
      yet). Debian branches keep calling `installed-to-live` and
      `/tmp/installed-to-live/cleanup.conf` checks unchanged.
- [ ] Once Arch users have validated `BindRootManager` for one or two
      releases, schedule a follow-up to flip Debian onto it and drop the
      `installed-to-live` runtime dependency.

**Risks accepted by deferring:**
- Cleanup/teardown logic stays duplicated between `installed-to-live` and
  `BindRootManager`; bug fixes must land in both.
- `Settings` ctor cleanup runs before `setVariables()` sets `isArch`, so that
  one site must use the compile-time `ARCH_BUILD` flag instead of the runtime
  flag. Easy to get wrong.
- `BindRootManager` testing surface stays narrow (Arch users only).
- Future upstream `installed-to-live` updates do not reach Arch automatically.

## Step 3 — Arch ISO/initrd code paths in `Work`

The Arch chain to cherry-pick from `arch`, in order:
`5bc9249` → `fa3bbd1` → `60f8f3e` → `5a07ae4` → `7bab12f` → `fb9a3da` →
`114671d` → `baef70c`.

- [x] `Work::copyNewIso`: branch on `isArch`. Keep `main`'s `--grub-mbr` and
      multi-init template logic in the `else` branch. Arch branch handles
      `/usr/lib/iso-template/arch/iso-template.tar.gz`, `.disk` UUID stamp,
      kernel + `archiso.img` copy to `boot/<i686|x86_64>/`, archiso initramfs
      rebuild via `mkinitcpio` when stale or missing.
- [x] Add private helpers `Work::kernelImageVersion`,
      `Work::initramfsKernelVersion`, `Work::rebuildArchisoInitramfs`.
- [x] `Work::createIso`: branch `squashfsPath` (`airootfs.sfs` for Arch vs
      `linuxfs` for MX), checksum (sha512 vs md5), xorriso volume label and
      boot args. Keep `main`'s `forceProgressSupported` detection and
      `-throttle` capability probe.
- [x] `Work::installPackage`: pacman branch (with `mx-installer` →
      `gazelle-installer` substitution and `checkInstalled` short-circuit).
- [x] `Work::checkInstalled`: pacman `-Q` branch.
- [x] `Work::savePackageList`: pacman branch writes
      `iso-template/arch/pkglist.<arch>.txt`.
- [x] `Work::setupEnv`: installer Desktop link logic — Arch only.
      MX continues to rely on mx-installer + `installed-to-live` to place the
      link via the skel-to-demo flow; `if (settings->isArch)` gate avoids
      redundant `ln -sf` on the MX path. Includes `/etc/skel/Desktop` symlink,
      demo home symlink under `resetAccounts`, and current-user `Desktop`
      symlink in non-reset mode with `installerLinkToRemove` cleanup
      tracking.
- [x] `helper.cpp` allow-list: merge in the additional commands from `arch`
      (`bash`, `chmod`, `cp`, `cat`, `adduser`, `deluser`, `install`, `ln`,
      `localize-repo`, `mkinitcpio`, `pacman`, `readlink`, `rm`, `sh`, `stat`,
      `touch`, `userdel`).
- [x] Smoke-test on MX: built an MX snapshot and it worked fine; no MX path
      regressed.
- [ ] Smoke-test on Arch (`-DARCH_BUILD=ON`): build a snapshot, boot the
      resulting ISO. *(Blocked on Step 2 — `setupEnv` still calls
      `installed-to-live` which is absent on Arch.)*

## Step 4 — Distro-agnostic hardening from `arch`

These fixes are not Arch-specific and should land regardless.

- [x] `cmd.cpp`: `disconnect(conn)` after the event loop (`9e304eb`).
- [x] `cmd.cpp` / `cmd.h`: add `setOutputSuppressed`/`outputSuppressed`,
      `runAsRoot`, `getOutAsRoot(cmd, quiet)` overload, `snapshotLibCommand`,
      `runSnapshotLib`. Reconcile with `main`'s untouched `Cmd`.
- [x] `main.cpp`: switch from `exit(EXIT_FAILURE)` calls to exception path
      with single `exitCode` at the bottom (`8668685`). Settings ctor and
      friends throw `std::runtime_error` instead of calling `exit()` so the
      catch in `main()` can record the exit code.
- [x] `Log::messageHandler`: thread-local reentrancy guard.
- [x] `MainWindow`: `closeEvent` override + `closeApp(fromCloseEvent)`
      rework. Geometry save preserved from main (`c38c52d`).
- [x] `MainWindow`: `cleanupInProgress` guard.
- [x] `Work`: change `cleanUp` from `[[noreturn]]` to non-returning-via-exit,
      add `cleanupStarted` guard, `requestExit` indirection. All `cleanUp()`
      call sites now have explicit `return` after them.
- [x] `Work::checkNoSpaceAndExit`: change to `[[nodiscard]] bool`, update
      callers in `checkEnoughSpace`.
- [x] `Batchprocessing`: insert `if (work.isCleaningUp()) return;` checkpoints
      between pipeline stages.
- [x] `FileSystemUtils::unsupportedPartitions`: extend with `9p`, `cifs`,
      `smbfs`, `fuse`, `fuseblk`, `fuse.vmhgfs-fuse`, `vmhgfs`, `vboxsf`,
      `virtiofs`.
- [x] `excludesutils::hasNvidiaGraphicsCard`: use `glxinfo` via `Cmd` while
      suppressing emitted output so the probe does not flood the log.
- [x] `SystemInfo::isLive`: also check `/run/archiso/bootmnt`.

## Step 5 — Packaging

Reordered from the original sequence: Step 5 substeps land 5a → 5c → 5d → 5b.

- [x] **5a:** Replace `build.sh` with arch's version (`--debian` / `--arch` /
      `--asan` modes); add `scripts-arch/snapshot-bootparameter.sh`; add
      `var/cache/pacman/pkg/*` and `var/lib/pacman/sync/*` to both exclude
      lists.
- [x] **5c:** Add `live-files/files` and `live-files/general-files` from
      arch (selectively staged — pre-existing `live-files/.claude/` is left
      untracked). Installed **only** via PKGBUILD; on Debian this content
      comes from the separate `mx-remaster-live-files` package. PKGBUILD
      keeps `Conflicts/Replaces: mx-remaster-live-files`.
- [x] **5d:** Keep the polkit helper policy files, but do **not** install
      separate `.rules` restrictions for the pkexec helpers. The policy
      `exec.path` annotation already selects `/usr/lib/<app>/helper`, and
      the policy default remains `auth_admin_keep`.
- [x] **Decision:** keep `polkit` policy defaults at `auth_admin_keep`.
      The compiled helper's allow-list remains the command boundary.
      A caller-executable restriction in `.rules` was rejected after smoke
      testing because the pkexec authorization subject is the pkexec request,
      not a reliable pointer back to the Qt frontend; returning
      `polkit.Result.NO` there caused a hard denial with no password prompt.
      Three things were rejected/fixed in the course of getting this right:
      - rejected the "root-owned caller → YES" branch from the arch
        version (almost any /usr/bin caller would have bypassed auth
        given the helper's broad allow-list);
      - rejected substring matching on the path (any path containing
        `mx-snapshot` anywhere, e.g. `/tmp/mx-snapshot-evil/foo`, would
        have satisfied the rule);
      - tried exact-path and basename matching first, then removed the
        rules because both variants still converted the policy default
        prompt into a no-prompt denial in real use.

      Bugs fixed along the way:
      - `polkit.spawn`'s return is a string, not an array; the arch
        rule's `caller_exe[0]` was reading the first character;
      - `subject.caller_pid` is undefined; the documented field is
        `subject.pid`. That still was not a suitable frontend executable
        check for pkexec helper invocations.
- [x] **5b:** Add `PKGBUILD`, `release.sh` (with `MAIN_BRANCH=main` default
      and `AUR_DIR` overridable). Arch packaging is **GUI-only**:
      `BUILD_CLI=OFF`, scripts installed only under
      `/usr/share/mx-snapshot/`. `scripts-arch/snapshot-bootparameter.sh`
      replaces the Debian `snapshot-bootparameter.sh`; `copy-initrd-*`
      are not installed on Arch (mkinitcpio handles initramfs). `aur/`
      is a separate sibling git repo (the AUR submission); not committed
      in this tree.
- [x] `mx-snapshot-exclude.list` and `iso-snapshot-cli-exclude.list`: add
      the Arch-specific lines (`var/cache/pacman/pkg/*`,
      `var/lib/pacman/sync/*`). Harmless on Debian. *(Landed with 5a.)*
- [ ] `debian/control`: leave `mx-remaster (>= 25.12.01)` Depends in place
      until Step 2 lands and removes the `installed-to-live` dependency.

## Step 6 — Translations

- [x] After all source merges land, run `lupdate` to regenerate `.ts` files.
      Did via `cmake --build build --target mx-snapshot_lupdate`. Result:
      260 source strings (17 net new from Steps 3 + 4 — Arch ISO/initrd
      messages, "Fatal error:" wrapper, "No supported filesystem found
      ...", etc.). 35 `.ts` files updated; large line diff is mostly
      message reordering by source location.
- [x] Verify intent of `arch`'s `mx-snapshot_en_US.ts` removal — diff it
      against `main`'s en_US first; if there's distinct content, preserve
      it. Diff'd: en_US has 245 messages vs en's 246, differing only in
      a single "Done" entry. en_US is effectively a duplicate of en —
      removing it (matching arch's decision).
- [ ] Notify translators that strings have shifted; do **not** hand-merge
      translation diffs.

## Step 7 — Branch lifecycle

- [ ] Smoke-test `unify` on MX-23 and MX-25 (Debian path).
- [ ] Smoke-test `unify` on Arch (Arch path).
- [ ] Smoke-test `unify` on MX-on-Arch if available (hybrid detection path).
- [ ] Fast-forward `main` to `unify`.
- [ ] Tag the previous `arch` tip as `arch-final`, then close/archive the
      `arch` branch.
- [ ] Update CI/release scripts to publish Debian + Arch packages from one
      tag.
- [~] Update `CLAUDE.md` and `README.md` to document the build matrix
      (`-DARCH_BUILD=ON` for Arch, default for Debian; runtime detection
      handles unusual setups).
      - `CLAUDE.md` updated locally with build matrix + runtime detection
        notes. The file is gitignored globally on this workstation, so
        the changes stay as local session context and are not committed.
      - `README.md` deliberately not touched yet. The current README is
        strongly worded as "MX/antiX only" and warns against installing
        the deb on other systems. Adding Arch documentation while Step 2
        (BindRootManager) is still deferred — i.e. while Arch runtime
        isn't feature-complete — would mislead users. Update the README
        when Step 2 lands and Arch is a fully supported runtime.

## Effort estimate

- Step 1: ~1 day
- Step 3: ~2 days (heavy merge conflicts with `--grub-mbr` and multi-init)
- Step 4: ~1 day
- Step 5: ~0.5 day
- Steps 6–7: ~0.5 day
- **Total without Step 2: ~5 days**
- Step 2 (later): ~3–5 days, mostly verification
