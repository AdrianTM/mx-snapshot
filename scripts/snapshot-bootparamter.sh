#!/bin/bash
#---------------------------------------------------------
# list boot parameter to be shown with boot paramter field
#---------------------------------------------------------

PROC_CMDLINE=$(cat /proc/cmdline)
CONF_CMDLINE=$(grep ^CONFIG_CMDLINE= /boot/config-$(uname -r) 2>/dev/null | cut -d\" -f2 | tail -1)

# find build in kernel parameter par to be ignored
CMD_LINE=""
for param in $PROC_CMDLINE; do
    x=x
    for conf in $CONF_CMDLINE; do
        [ "$conf" = "$param" ] && x="" && break
    done
    [ -n "$x" ] &&  CMD_LINE="$CMD_LINE $param"
done

# regexp to get all kernel and live boot parameter 
# inclding any quoted with spaces

par_list=$(echo "$CMD_LINE " | 
    grep -oP '((?<=\s)|^)[[:alnum:]._-]+(=([^\s\n]*?)?|)(?=[\s\n])|((?<=\s)|^)("[[:alnum:]._-]+=[^"]*?")' | 
    sed -r 's/"([[:alnum:]._-]+=)/\1"/')

readarray -t PAR_LIST <<<"$par_list"

OUT_LIST=()

for param in "${PAR_LIST[@]}"; do
    case "$param" in

                          menus)  ;;
                        menus=*)  ;;
                       old-conf)  ;;
                       new-conf)  ;;
                         splash)  ;;
                       splash=*)  ;;
                       poweroff)  ;;
                         reboot)  ;;
               bootdir=*|bdir=*)  ;;
    bootlabel=*|blabel=*|blab=*)  ;;
             bootuuid=*|buuid=*)  ;;
               bootdev=*|bdev=*)  ;;

                  try=*|retry=*)  ;;

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

           verbose=*|verb=*|v=*)  ;;
                           bp=*)  ;;
                           pk=*)  ;;

             check|md5|checkmd5)  ;;
                 hico|highcolor)  ;;
                  loco|lowcolor)  ;;
                   noco|nocolor)  ;;

                         noxtra)  ;;
                         doxtra)  ;;

                            db+)  ;;
                           db++)  ;;

            fancyprompt|fprompt)  ;;
               autologin|alogin)  ;;

#                  toram|toram=*)  ;;

                     noremaster)  ;;
                       rollback)  ;;
#                         lang=*)  ;;

                        noclear)  ;;
              gfxsave|gfxsave=*)  ;;

                        checkfs)  ;;
                      nocheckfs)  ;;

                       failsafe)  ;;
                         load=*)  ;;
#               bl=*|blacklist=*)  ;;

                       coldplug)  ;;
                        hotplug)  ;;
                      traceload)  ;;
                 autoload|aload)  ;;

                         init=*)  ;;

                  nousb2|noehci)  ;;

                       noerrlog)  ;;
                        errtest)  ;;
                      errtest=*)  ;;
                        noerr=*)  ;;

                      vtblank=*)  ;;

               livedir=*|ldir=*)  ;;
        pw|passwd|pw=*|passwd=*)  ;;
              private|private=*)  ;;
                      bootchart)  ;;
                        noplink)  ;;
                          plink)  ;;

                  disable=*|nosysv=*)  ;;
     disable_srv=*|disable_service=*)  ;;
           lean|mean|Xtralean|nodbus)  ;;
        initcall_debug|printk.time=y)  ;;

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
                        nostore)  ;;
                        dostore)  ;;
                   disablestore)  ;;
              fatuid=*|fatgid=*)  ;;
                    live_swap=*)  ;;
                   mk_swap_file)  ;;
                 mk_swap_file=*)  ;;
                       notmptmp)  ;;
                    nomicrocode)  ;;
                     nosavelogs)  ;;
                         tty1on)  ;;
#                        splasht)  ;;
#                      splasht=*)  ;;
                       st_tty=*)  ;;
                        vcard=*)  ;;

        # Our Live params
#        hwclock=utc|hwclock=local|xorg|xorg=*|noearlyvid|earlyvid) ;;
#        amnt|amnt=*|automount|automount=*|confont=*|conkeys=*);;
#        desktop=*|dpi=*|fstab=*|hostname=*|kbd=*|kbopt=*|kbvar=*);;
#        lang=*|mirror=*|mount=*|noloadkeys|noprompt);;
#        nosplash|password|password=*|prompt|pw|pw=*|tz=*|ubp=*|ushow=*);;
        nosplash|password|password=*|prompt|pw|pw=*|ubp=*|ushow=*);;
        uverb=*|xres=*|noxorg);;
        desktheme=*) ;;
        nosavestate|savestate|dbsavestate) ;;
        deskdelay=*) ;;
        udpi=*|sdpi=*) ;;
        fontsize=*) ;;
#        norepo|norepo=*) ;;
        conwidth=*);;
#        noautomount) ;;
        vga=ask+save) ;;

        skylakeflicker)  ;;
        i915powersave) ;;

#       aufs|overlayfs) ;;
#       wicd|nowicd) ;;

        nomicrocode) ;;
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

        # luks paramter
            [bfp]luks*) ;;
        # anything else
           quiet|splash) ;;

        *) OUT_LIST+=("$param")
    esac
done

# revert list REV_LIST to keep last parameter only
readarray -t REV_LIST <<<$( printf "%s\n" "${OUT_LIST[@]}" | tac )

# list of key-values paramter to keep only the last
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
 
declare -A SEEN 
OUT_PAR=()
for par in "${REV_LIST[@]}"; do
	key="${par}"
	if [ "${par}" != "${par%%=*}" ]; then
	   [ ${KEYS["${par%%=*}"]+set} ] && key="${par%%=*}"
	fi
	if [ ${SEEN["$key"]+set} ]; then
		echo "seen: $par " 
		continue 
	fi
	SEEN["$key"]="$key"
	OUT_PAR+=("$par")
done

OUT_LIST=()

readarray -t OUT_LIST <<<$( printf "%s\n" "${OUT_PAR[@]}" | tac )
printf "%s\n" "${OUT_LIST[@]}"

