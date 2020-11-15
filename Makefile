version=$(shell cat VERSION)
pkgname=minisphere-$(version)
os=$(shell uname)

ifndef prefix
ifeq ($(os), Darwin)
prefix=/usr/local
else
prefix=/usr
endif
endif
installdir=$(DESTDIR)$(prefix)

ifndef CC
CC=cc
endif

ifndef CFLAGS
CFLAGS=-O3
endif

engine_sources=src/minisphere/main.c \
   src/shared/api.c \
   src/shared/compress.c \
   src/shared/console.c \
   src/shared/dyad.c \
   src/shared/encoding.c \
   src/shared/jsal.c \
   src/shared/ki.c \
   src/shared/lstring.c \
   src/shared/md5.c \
   src/shared/path.c \
   src/shared/sockets.c \
   src/shared/unicode.c \
   src/shared/vector.c \
   src/shared/wildmatch.c \
   src/shared/xoroshiro.c \
   src/minisphere/animation.c \
   src/minisphere/atlas.c \
   src/minisphere/audio.c \
   src/minisphere/blend_op.c \
   src/minisphere/byte_array.c \
   src/minisphere/color.c \
   src/minisphere/debugger.c \
   src/minisphere/dispatch.c \
   src/minisphere/event_loop.c \
   src/minisphere/font.c \
   src/minisphere/galileo.c \
   src/minisphere/game.c \
   src/minisphere/geometry.c \
   src/minisphere/image.c \
   src/minisphere/input.c \
   src/minisphere/kev_file.c \
   src/minisphere/legacy.c \
   src/minisphere/logger.c \
   src/minisphere/map_engine.c \
   src/minisphere/module.c \
   src/minisphere/obstruction.c \
   src/minisphere/package.c \
   src/minisphere/pegasus.c \
   src/minisphere/profiler.c \
   src/minisphere/screen.c \
   src/minisphere/script.c \
   src/minisphere/source_map.c \
   src/minisphere/spriteset.c \
   src/minisphere/table.c \
   src/minisphere/tileset.c \
   src/minisphere/transform.c \
   src/minisphere/utility.c \
   src/minisphere/vanilla.c \
   src/minisphere/windowstyle.c
engine_libs= \
   -lallegro_acodec \
   -lallegro_audio \
   -lallegro_color \
   -lallegro_dialog \
   -lallegro_font \
   -lallegro_image \
   -lallegro_main \
   -lallegro_memfile \
   -lallegro_primitives \
   -lallegro_ttf \
   -lallegro \
   -lChakraCore \
   -lz \
   -lm

cell_sources=src/cell/main.c \
   src/shared/api.c \
   src/shared/compress.c \
   src/shared/encoding.c \
   src/shared/jsal.c \
   src/shared/lstring.c \
   src/shared/path.c \
   src/shared/unicode.c \
   src/shared/vector.c \
   src/shared/wildmatch.c \
   src/shared/xoroshiro.c \
   src/cell/build.c \
   src/cell/fs.c \
   src/cell/image.c \
   src/cell/module.c \
   src/cell/spk_writer.c \
   src/cell/target.c \
   src/cell/tileset.c \
   src/cell/tool.c \
   src/cell/utility.c \
   src/cell/visor.c
cell_libs= \
   -lChakraCore \
   -lpng \
   -lz \
   -lm

ssj_sources=src/ssj/main.c \
   src/shared/console.c \
   src/shared/dyad.c \
   src/shared/ki.c \
   src/shared/path.c \
   src/shared/sockets.c \
   src/shared/vector.c \
   src/shared/xoroshiro.c \
   src/ssj/backtrace.c \
   src/ssj/help.c \
   src/ssj/inferior.c \
   src/ssj/listing.c \
   src/ssj/objview.c \
   src/ssj/parser.c \
   src/ssj/session.c

ifeq ($(os), Darwin)
LINKER_ARGS=-Wl,-rpath,\$$ORIGIN
CHAKRACORE_URL=https://aka.ms/chakracore/cc_osx_x64_1_11_15
else
LINKER_ARGS=-Wl,-rpath=\$$ORIGIN
OPTIONS=-DMINISPHERE_MNG_SUPPORT
engine-libs+=-lmng
CHAKRACORE_URL=https://aka.ms/chakracore/cc_linux_x64_1_11_15
endif

.PHONY: all
all: minisphere spherun cell ssj

.PHONY: deps
deps:
	mkdir -p dep
	wget -O dep/libChakraCore.tar.gz $(CHAKRACORE_URL)
	cd dep && tar xzf libChakraCore.tar.gz --strip-components=1 ChakraCoreFiles/include ChakraCoreFiles/lib

.PHONY: installdeps
installdeps:
	cp dep/lib/* $(installdir)/lib

.PHONY: minisphere
minisphere: bin/minisphere

.PHONY: spherun
spherun: bin/minisphere bin/spherun

.PHONY: cell
cell: bin/cell

.PHONY: ssj
ssj: bin/ssj

.PHONY: dist
dist: all
	mkdir -p dist/$(pkgname)
	cp -r assets desktop docs license manpages src dist/$(pkgname)
	cp Makefile VERSION dist/$(pkgname)
	cp CHANGELOG.md LICENSE.txt README.md dist/$(pkgname)
	cd dist && tar czf $(pkgname).tar.gz $(pkgname) && rm -rf dist/$(pkgname)

.PHONY: install
install: all
	mkdir -p $(installdir)/bin
	mkdir -p $(installdir)/lib
	mkdir -p $(installdir)/share/minisphere
	mkdir -p $(installdir)/share/applications
	mkdir -p $(installdir)/share/doc/minisphere
	mkdir -p $(installdir)/share/icons/hicolor/scalable/mimetypes
	mkdir -p $(installdir)/share/mime/packages
	mkdir -p $(installdir)/share/man/man1
	mkdir -p $(installdir)/share/pixmaps
	cp bin/minisphere bin/spherun bin/cell bin/ssj $(installdir)/bin
	cp -r bin/system $(installdir)/share/minisphere
	gzip docs/sphere2-core-api.txt -c > $(installdir)/share/doc/minisphere/sphere2-core-api.gz
	gzip docs/sphere2-hl-api.txt -c > $(installdir)/share/doc/minisphere/sphere2-hl-api.gz
	gzip docs/cellscript-api.txt -c > $(installdir)/share/doc/minisphere/cellscript-api.gz
	gzip manpages/minisphere.1 -c > $(installdir)/share/man/man1/minisphere.1.gz
	gzip manpages/spherun.1 -c > $(installdir)/share/man/man1/spherun.1.gz
	gzip manpages/cell.1 -c > $(installdir)/share/man/man1/cell.1.gz
	gzip manpages/ssj.1 -c > $(installdir)/share/man/man1/ssj.1.gz
	cp desktop/minisphere.desktop $(installdir)/share/applications
	cp desktop/sphere-icon.svg $(installdir)/share/pixmaps
	cp desktop/mimetypes/minisphere.xml $(installdir)/share/mime/packages
	cp desktop/mimetypes/*.svg $(installdir)/share/icons/hicolor/scalable/mimetypes

.PHONY: clean
clean:
	rm -rf bin
	rm -rf dist

bin/minisphere:
	mkdir -p bin
	$(CC) -o bin/minisphere $(CFLAGS) \
	      -fno-omit-frame-pointer \
	      -Idep/include -Isrc/shared -Isrc/minisphere \
	      -Ldep/lib \
	      $(LINKER_ARGS) \
	      $(OPTIONS) \
	      $(engine_sources) $(engine_libs)
	cp -r assets/system bin

bin/spherun:
	mkdir -p bin
	$(CC) -o bin/spherun $(CFLAGS) \
	      -fno-omit-frame-pointer \
	      -Idep/include -Isrc/shared -Isrc/minisphere \
	      -Ldep/lib \
	      $(LINKER_ARGS) \
	      $(OPTIONS) \
	      -DMINISPHERE_SPHERUN \
	      $(engine_sources) $(engine_libs)

bin/cell:
	mkdir -p bin
	$(CC) -o bin/cell $(CFLAGS) \
	      -fno-omit-frame-pointer \
	      -Idep/include -Isrc/shared \
	      -Ldep/lib \
	      $(LINKER_ARGS) \
	      $(cell_sources) $(cell_libs)

bin/ssj:
	mkdir -p bin
	$(CC) -o bin/ssj $(CFLAGS) -Isrc/shared $(ssj_sources)
