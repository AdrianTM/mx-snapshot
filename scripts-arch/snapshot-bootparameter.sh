#!/usr/bin/env bash

# This script is part of MX Snapshot
#
# to list the boot parameters to be displayed as boot options.
#---------------------------------------------------------
VERSION="241424"

#---------------------------------------------------------
# Allow debug
[[ "$1" = "-d" ||  "$1" = "--debug" ]] && SNAPSHOT_BOOTPARAMETER_DEBUG=true

[[ -z "${SNAPSHOT_BOOTPARAMETER_DEBUG}" ]] ||  SNAPSHOT_BOOTPARAMETER_DEBUG=true
SBP_DEBUG=$SNAPSHOT_BOOTPARAMETER_DEBUG

#---------------------------------------------------------
# Some globals
#---------------------------------------------------------

# Default keyboard options
DEFAULT_KBOPT_MX='grp:rctrl_rshift_toggle,terminate:ctrl_alt_bksp,grp_led:scroll'
DEFAULT_KBOPT_ANTIX='grp:lalt_lshift_toggle,terminate:ctrl_alt_bksp,grp_led:scroll'

# Some lists
unset CONF_LIST CONF_HASH OUT_LIST PAR_LIST CONF_HASH
declare -a CONF_LIST OUT_LIST PAR_LIST
# Hash of config cmdline paramter
declare -A CONF_HASH

unset KBD      ; declare -A KBD
unset KBD_LIST ; declare -a KBD_LIST

#---------------------------------------------------------
# Allow override
: "${PROC_CMDLINE:=$(</proc/cmdline)}"
: "${CONF_CMDLINE:=$(grep -m1 '^CONFIG_CMDLINE=' /boot/config-"$(uname -r)" 2>/dev/null | sed -E 's/^CONFIG_CMDLINE="(.*)"/\1/')}"

#-----------------------------------------------------------------------
main() {
#-----------------------------------------------------------------------
    prepare_keyboard
    prepare_language
    prepare_timezone
    prepare_par_list

    local param
    for param in "${PAR_LIST[@]}"; do
        # Built-in kernel parameters that should be ignored
        [[ -v CONF_HASH["$param"] ]] && continue
        case "$param" in
                              menus)  ;;
                            menus=*)  ;;
                           old-conf)  ;;
                           new-conf)  ;;
    #                        splash)  ;;
    #                      splash=*)  ;;
                           poweroff)  ;;
                             reboot)  ;;
                   bootdir=*|bdir=*)  ;;
        bootlabel=*|blabel=*|blab=*)  ;;
                 bootuuid=*|buuid=*)  ;;
                   bootdev=*|bdev=*)  ;;

    #                 try=*|retry=*)  ;;

                persistdir=*|pdir=*)  ;;

     persistlabel=*|plabel=*|plab=*)  ;;
              persistuuid=*|puuid=*)  ;;
                persistdev=*|pdev=*)  ;;

     persistretry=*|pretry=*|ptry=*)  ;;

                              dbv=*)  ;;
                            fneed=*)  ;;
                             fforce)  ;;
                             frugal)  ;;
                           frugal=*)  ;;
                             flab=*)  ;;
                             fdev=*)  ;;
                            fuuid=*)  ;;

                    iso=*|fromiso=*)  ;;
                        iso|fromiso)  ;;

                             from=*)  ;;

                      persist=*|p=*)  ;;
                            persist)  ;;

                            sqext=*)  ;;
                           sqname=*)  ;;
                               sq=*)  ;;

    #          verbose=*|verb=*|v=*)  ;;
                               bp=*)  ;;
                               pk=*)  ;;

                 check|md5|checkmd5)  ;;
    #                hico|highcolor)  ;;
    #                 loco|lowcolor)  ;;
    #                  noco|nocolor)  ;;

    #                        noxtra)  ;;
    #                        doxtra)  ;;

                                db+)  ;;
                               db++)  ;;

    #           fancyprompt|fprompt)  ;;
    #              autologin|alogin)  ;;

    #                 toram|toram=*)  ;;

                         noremaster)  ;;
                           rollback)  ;;
    #                        lang=*)  ;;

    #                       noclear)  ;;
                  gfxsave|gfxsave=*)  ;;

    #                       checkfs)  ;;
    #                     nocheckfs)  ;;

    #                      failsafe)  ;;
    #                        load=*)  ;;
    #              bl=*|blacklist=*)  ;;

    #                      coldplug)  ;;
    #                       hotplug)  ;;
    #                     traceload)  ;;
    #                autoload|aload)  ;;

    #                        init=*)  ;;

    #                 nousb2|noehci)  ;;

    #                      noerrlog)  ;;
    #                       errtest)  ;;
    #                     errtest=*)  ;;
    #                       noerr=*)  ;;

    #                     vtblank=*)  ;;

                   livedir=*|ldir=*)  ;;
                  private|private=*)  ;;
    #                     bootchart)  ;;
    #                       noplink)  ;;
    #                         plink)  ;;

    #                  disable=*|nosysv=*)  ;;
    #     disable_srv=*|disable_service=*)  ;;
    #           lean|mean|Xtralean|nodbus)  ;;
    #        initcall_debug|printk.time=y)  ;;

            # Convenience short cuts from our menus
                        persist_all)  ;;
                       persist_root)  ;;
                     persist_static)  ;;
                     p_static_root)   ;;
                       persist_home)  ;;
                     frugal_persist)  ;;
                        frugal_root)  ;;
                      frugal_static)  ;;
                      f_static_root)  ;;
                        frugal_home)  ;;
                        frugal_only)  ;;
    #                       nostore)  ;;
    #                       dostore)  ;;
                       disablestore)  ;;
                  fatuid=*|fatgid=*)  ;;
                      # live_swap=*)  ;;
    #                  mk_swap_file)  ;;
    #                mk_swap_file=*)  ;;
    #                      notmptmp)  ;;
    #                   nomicrocode)  ;;
    #                    nosavelogs)  ;;
    #                        tty1on)  ;;
    #                       splasht)  ;;
    #                     splasht=*)  ;;
    #                      st_tty=*)  ;;
    #                       vcard=*)  ;;

            # Our Live params
    #        hwclock=utc|hwclock=local|xorg|xorg=*|noearlyvid|earlyvid) ;;
    #        amnt|amnt=*|automount|automount=*|confont=*|conkeys=*);;
    #        desktop=*|dpi=*|fstab=*|hostname=*|kbd=*|kbopt=*|kbvar=*);;
    #        lang=*|mirror=*|mount=*|noloadkeys|noprompt);;
    #        nosplash|password|password=*|prompt|pw|pw=*|tz=*|ubp=*|ushow=*);;
            password|password=*|prompt|pw|pw=*|ubp=*|ushow=*|passwd|passwd=*)  ;;
    #        uverb=*|xres=*|noxorg);;
    #        desktheme=*) ;;
    #        nosavestate|savestate|dbsavestate) ;;
    #        deskdelay=*) ;;
    #        udpi=*|sdpi=*) ;;
    #        fontsize=*) ;;
    #        norepo|norepo=*) ;;
    #        conwidth=*);;
    #        noautomount) ;;
    #        vga=ask+save) ;;

    #        skylakeflicker)  ;;
    #        i915powersave) ;;
    #       aufs|overlayfs) ;;
    #       wicd|nowicd) ;;

    #        nomicrocode) ;;
    #        live_swap=off)  ;;
    #        live_swap=force)  ;;
    #        live_swap=all-off)  ;;
    #        mk_swap_file=*)     ;;

            # Added by Grub
            BOOT_IMAGE=*) ;;

            # Sometimes added with grub loop mounts
                *root=*) ;;

            # Added by live-Grub menu
            grubsave)  ;;
            kernel=*)  ;;
             extra=*)  ;;

            # Luks parameter
                [bfp]luks*) ;;

            # Mount root device read-only on boot
                ro) ;;

            # Btrfs rootflags
               rootflags|rootflags=*) ;;

            # Hibernate and resume parameter
               hibernate=*|nohibernate) ;;
               resume*) ;;

            # Default inits
            init=/sbin/init) ;;
            init=/usr/sbin/init) ;;

            # Other inits only for supported live system currently available within mx-iso-template
            # init=*)
            #    if dpkg --compare-versions "$(dpkg-query -f '${Version}' -W mx-iso-template)" ge "24.03.01mx23"; then
            #       OUT_LIST+=("$(vquote "$param")")
            #    fi
            #    ;;

            init=*) ;;

            # Remove the nvidia boot option that may have been added with a previous snapshot
            xorg=nvidia) ;;

            *) OUT_LIST+=("$(vquote "$param")")
        esac
    done

    [[ -z "$tz" ]] ||  OUT_LIST+=("$(vquote "tz=$tz")")

    # Revert list REV_LIST to keep last parameter only
    unset REV_LIST KEYS KEYS_LIST OUT_PAR
    REV_LIST=()

    [[ -z "$language" ]] || REV_LIST=(lang="$language")

    i=${#OUT_LIST[@]};
    while ((i--)); do REV_LIST+=("${OUT_LIST[$i]}"); done

    # List of parameters to keep only the last set

    KEY_LIST=(
        lang
        kbd kbopt kbvar
        tz
        toram
        splasht
        )

    declare -A KEYS
    for key in "${KEY_LIST[@]}"; do
        KEYS["$key"]="$key"
    done

    unset SEEN
    declare -A SEEN
    OUT_PAR=()

    for par in "${REV_LIST[@]}"; do
        key="${par%%=*}"
        [[ -n "$key" ]] || continue
        [[ ${KEYS["$key"]+set} ]] && [[ ${SEEN["$key"]+set} ]] &&  continue
        SEEN["$key"]="$par"
        OUT_PAR+=("$par")
    done
    # List of parameters to display only the last set
    unset KEY_LAST
    KEY_LAST=(
        lang
        kbd kbvar
        kbopt
        tz
        )
    unset LAST_KEYS
    declare -A LAST_KEYS
    for key in "${KEY_LAST[@]}"; do
        LAST_KEYS["$key"]="$key"
    done

    # revert list
    i=${#OUT_PAR[@]};
    while ((i--)); do
        par=${OUT_PAR[$i]}
        key="${par%%=*}"
        [[ -v LAST_KEYS["$key"] ]] && continue
        printf '%s\n' "${par}"
    done


    [[ -e /etc/antix-version ]] && KBOPT_DEFAULT="$DEFAULT_KBOPT_ANTIX"
    [[ -e /etc/mx-version    ]] && KBOPT_DEFAULT="$DEFAULT_KBOPT_MX"

    local kbopt_sorted
    local kbopt_sorted_default

    for key in "${KEY_LAST[@]}"; do
        [[ -v SEEN["$key"] ]] || continue
        if [[ "$key" = "kbopt" ]]; then
           local seen="${SEEN["$key"]}"
           seen="${seen#kbopt=}"
           kbopt_sorted=$(sorted_opts "$seen")
           kbopt_sorted_default=$(sorted_opts "${KBOPT_DEFAULT}")
           [[ "$kbopt_sorted" = "${kbopt_sorted_default}" ]] && continue
        fi
        par=${SEEN["$key"]}
        val="${par##*=}"
        [[ -n "$val" ]] || continue
        printf '%s\n' "${par}"
    done
}

#---------------------------------------------------------
# Some functions
#---------------------------------------------------------

#----------------------------------------------------------------------------
# Combine boot parameter with system keyboard parameter
prepare_par_list() {
    PROC_CMDLINE=$(sanitize_bootparameter "$PROC_CMDLINE")
    CONF_CMDLINE=$(sanitize_bootparameter "$CONF_CMDLINE")

    #debug PROC_CMDLINE="'$PROC_CMDLINE'"
    #debug CONF_CMDLINE="'$CONF_CMDLINE'"
    debug_param PROC_CMDLINE
    debug_param CONF_CMDLINE

    [[ -n "$PROC_CMDLINE" ]] && readarray -d ' ' -t PAR_LIST <<< "$PROC_CMDLINE"
    debug_list PAR_LIST

    [[ -n "$CONF_CMDLINE" ]] && readarray -d ' ' -t CONF_LIST <<< "$CONF_CMDLINE"
    debug_list CONF_LIST

    debug_list PAR_LIST
    debug_list KBD_LIST

    PAR_LIST+=("${KBD_LIST[@]}")
    for p in "${CONF_LIST[@]}"; do
        CONF_HASH["$p"]=x
    done
    debug_list PAR_LIST
}

#----------------------------------------------------------------------------
# Get system keyboard layout from /etc/default/keyboard
prepare_keyboard() {
    local rex='^((xkb)?(layout|options|variant))([:=]["]?[[:space:]]*)([^"[:space:]]+)["]?'
    local layout

:<<'NotUsed'
    if false && setxkbmap -query 1>/dev/null  2>&1; then
        layout=$(setxkbmap -query 2>/dev/null)
    else
    else
        layout=$(sed '/^XKB/! d; s/^XKB//;' /etc/default/keyboard 2>/dev/null \
                | tr '[:upper:]' '[:lower:]')
    fi
NotUsed

    layout=$(sed '/^XKB/! d; s/^XKB//;' /etc/default/keyboard 2>/dev/null \
            | tr '[:upper:]' '[:lower:]')

    unset LAYOUT
    # shellcheck disable=SC1083,SC2046
    eval "declare -A LAYOUT=( $(sed -nr "s/$rex/\"\1\" \"\5\"/p" <<< "$layout") )"

    debug_list "LAYOUT"
    unset KEY_MAP
    declare -A KEY_MAP=(
        [layout]=kbd
        [variant]=kbvar
        [options]=kbopt
    )
    debug_list KEY_MAP


    for l in "${!KEY_MAP[@]}"; do
        k=${KEY_MAP[$l]}
        # shellcheck disable=SC2153
        KBD[$k]="${LAYOUT[$l]}"
        KBD_LIST+=("$k=${KBD[$k]}")
    done
    debug_list KBD
    debug_list KBD_LIST
    }

#----------------------------------------------------------------------------
# Get system language from /etc/default/locale
prepare_language() {
    local lang
    local default_locale=/etc/default/locale
    language=
    if [[ -r "$default_locale" ]]; then
        lang=$(grep ^LANG=  "$default_locale" | tr -d '"' | tail -1)
        lang=$( grep ^LANG= "$default_locale" 2>/dev/null | \
                tail -1 | tr -d '"' | \
                sed -nr 's/LANG=//; s/^([[:alpha:]_]+).*/\1/p' )
    fi
    [[ -n "$lang" ]] && language="$lang"

}

#----------------------------------------------------------------------------
# Get current system timezone from /etc/timezone
prepare_timezone() {
    # Allow TZ override
    tz=$TZ
    [[ -z "$TZ" ]] && [[ -r /etc/timezone ]] && tz="$(head -1 /etc/timezone)"
    [[ -n "$tz" ]] || tz=
}

#----------------------------------------------------------------------------
# Clean bad chars from user adjusted boot parameters
sanitize_bootparameter() {
    local p=$1
    # "bad" characters that we do not want in the boot parameter
    # shellcheck disable=SC2016
    local bad_chars='<>$()[]`|'
    printf '%s' "$(LC_ALL=C tr -d -- "$bad_chars"  <<<"$p")"
}

#----------------------------------------------------------------------------
# Double-quote values with spaces
vquote() {
    local c=$1 p v
    p=${c%%=*}
    [[ "$p" !=  "$c" ]] && v=${c#*=}
    if [[ "$v" !=  "${v#* }" ]] ; then
        printf '%s="%s"\n' "$p" "$v"
    else
        printf '%s\n' "$c"
    fi
}

#----------------------------------------------------------------------------
# Print debug to stderr
debug() {
    [[ "$SBP_DEBUG" ]] || return
    local s="$*"
    printf '[DEBUG]: %s\n' "${s}" >&2
}

#----------------------------------------------------------------------------
# Print debug of parameter to stderr
debug_param() {
    [[ "$SBP_DEBUG" ]] || return
    local param=$1
    [[ -n "$param" ]] || return
    local -n name=$param 2>/dev/null || return
    local s="$name"
    printf "[DEBUG]: $param=%s\n" "${s@Q}" >&2
}

#----------------------------------------------------------------------------
# Print debug of list or hash to stderr
debug_list() {
    [[ "$SBP_DEBUG" ]] || return
    local list=$1
    [[ -n "$list" ]] || return
    local -n name=$list
    local s="${name[*]@Q}"
    s=$(sanitize_bootparameter "$s")
    printf "[DEBUG]: $list=(%s)\n" "$s" >&2
}

#----------------------------------------------------------------------------
# Sort keybord options
sorted_opts() {
    printf '%s\n' "${1//,/ }" | sort -u | paste -sd, -
}

#----------------------------------------------------------------------------
# Main
main "$@"
