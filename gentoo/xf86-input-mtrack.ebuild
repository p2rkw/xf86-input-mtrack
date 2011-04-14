# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=3

#inherit x-modular
inherit eutils

DESCRIPTION="Multitouch-X-Driver for MT-Touchpads"
HOMEPAGE="http://bitmath.org/code/multitouch/"
COMMIT="fc4d145"
SRC_URI="http://github.com/rydberg/Multitouch-X-Driver/tarball/v1.0-rc2 -> ${P}.tar.gz"
IUSE="debug"

#S="${WORKDIR}/rydberg-Multitouch-X-Driver-${COMMIT}"
S="${WORKDIR}/multitouch-${PV}"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"

RDEPEND="
	>=x11-base/xorg-server-1.8.0
	>=sys-libs/mtdev-1.0"
DEPEND="${RDEPEND}
	x11-proto/inputproto"

#src_prepare() {
#	epatch "${FILESDIR}/gentoo-${PV}.patch"
#}

src_compile() {
	CFLAGS=""
	if use debug; then
		CFLAGS="${CFLAGS} -DDEBUG_GESTURES=1 -DDEBUG_DRIVER=1"
	fi
	emake || die "emake failed"
}

src_install() {
	emake DESTDIR="${D}" install || die "emake install failed"
}

pkg_postinst() {
	echo
	elog "To enable multitouch support add the following lines"
	elog "to your xorg.conf:"
	elog ""
	elog "Section \"InputClass\""
	elog "  MatchIsTouchpad \"true\""
	elog "  Identifier      \"Touchpads\""
	elog "  Driver          \"multitouch\""
	elog "EndSection"
	elog ""
}
