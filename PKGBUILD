# Maintainer: Adrian <adrian@mxlinux.org>
pkgname=mx-snapshot
pkgver=${PKGVER:-25.09.1}
pkgrel=1
pkgdesc="A tool for creating live ISO images from running systems"
arch=('x86_64' 'i686')
url="https://mxlinux.org"
license=('GPL3')
depends=('qt6-base' 'polkit' 'squashfs-tools' 'xorriso' 'mx-iso-template-arch')
makedepends=('cmake' 'ninja' 'qt6-tools')
source=()
sha256sums=()

build() {
    cd "${startdir}"

    rm -rf build

    cmake -G Ninja \
        -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DARCH_BUILD=ON \
        -DBUILD_GUI=ON \
        -DBUILD_CLI=ON

    cmake --build build --parallel
}

package() {
    cd "${startdir}"

    install -Dm755 build/mx-snapshot "${pkgdir}/usr/bin/mx-snapshot"
    install -Dm755 build/iso-snapshot-cli "${pkgdir}/usr/bin/iso-snapshot-cli"

    install -dm755 "${pkgdir}/usr/share/mx-snapshot/locale"
    install -dm755 "${pkgdir}/usr/share/iso-snapshot-cli/locale"
    install -Dm644 build/*.qm "${pkgdir}/usr/share/mx-snapshot/locale/" 2>/dev/null || true
    install -Dm644 build/*.qm "${pkgdir}/usr/share/iso-snapshot-cli/locale/" 2>/dev/null || true

    install -dm755 "${pkgdir}/usr/share/mx-snapshot/scripts"
    install -dm755 "${pkgdir}/usr/share/iso-snapshot-cli/scripts"
    cp -a scripts-arch/* "${pkgdir}/usr/share/mx-snapshot/scripts/"
    cp -a scripts-arch/* "${pkgdir}/usr/share/iso-snapshot-cli/scripts/"

    ln -sf "/usr/share/mx-snapshot/scripts/arch-remaster" "${pkgdir}/usr/bin/arch-remaster"
    ln -sf "/usr/share/iso-snapshot-cli/scripts/arch-remaster" "${pkgdir}/usr/bin/arch-remaster-cli"

    install -dm755 "${pkgdir}/usr/lib/mx-snapshot"
    install -dm755 "${pkgdir}/usr/lib/iso-snapshot-cli"
    install -Dm755 polkit/helper "${pkgdir}/usr/lib/mx-snapshot/helper"
    install -Dm755 polkit/snapshot-lib "${pkgdir}/usr/lib/mx-snapshot/snapshot-lib"
    install -Dm755 polkit/helper "${pkgdir}/usr/lib/iso-snapshot-cli/helper"
    install -Dm755 polkit/snapshot-lib "${pkgdir}/usr/lib/iso-snapshot-cli/snapshot-lib"

    install -dm755 "${pkgdir}/usr/share/polkit-1/actions"
    for policy in polkit/*.policy; do
        install -Dm644 "$policy" "${pkgdir}/usr/share/polkit-1/actions/$(basename "$policy")"
    done

    install -Dm644 mx-snapshot.desktop "${pkgdir}/usr/share/applications/mx-snapshot.desktop"
    install -Dm644 icons/mx-snapshot.png "${pkgdir}/usr/share/icons/hicolor/48x48/apps/mx-snapshot.png"
    install -Dm644 icons/mx-snapshot.svg "${pkgdir}/usr/share/icons/hicolor/scalable/apps/mx-snapshot.svg"
    install -Dm644 icons/mx-snapshot.png "${pkgdir}/usr/share/pixmaps/mx-snapshot.png"

    install -Dm644 mx-snapshot.conf "${pkgdir}/etc/mx-snapshot.conf"
    install -Dm644 iso-snapshot-cli.conf "${pkgdir}/etc/iso-snapshot-cli.conf"
    install -Dm644 mx-snapshot-exclude.list "${pkgdir}/etc/mx-snapshot-exclude.list"
    install -Dm644 iso-snapshot-cli-exclude.list "${pkgdir}/etc/iso-snapshot-cli-exclude.list"

    install -dm755 "${pkgdir}/usr/share/excludes"
    install -Dm644 mx-snapshot-exclude.list "${pkgdir}/usr/share/excludes/mx-snapshot-exclude.list"
    install -Dm644 iso-snapshot-cli-exclude.list "${pkgdir}/usr/share/excludes/iso-snapshot-cli-exclude.list"

    install -Dm644 manual/mx-snapshot.8 "${pkgdir}/usr/share/man/man8/mx-snapshot.8"
    install -Dm644 manual/iso-snapshot-cli.8 "${pkgdir}/usr/share/man/man8/iso-snapshot-cli.8"

    install -dm755 "${pkgdir}/usr/share/doc/mx-snapshot"
    if [ -d docs ]; then
        cp -r docs/* "${pkgdir}/usr/share/doc/mx-snapshot/" 2>/dev/null || true
    fi

}
