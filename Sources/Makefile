
# !!! FIXME: Make this more robust. MUCH more robust.
# !!! FIXME: ...or at least comment the rest of these options...

# icc is intel's compiler. gcc is, well.., gcc.
# Intel compiler < version 6.0 probably won't work.
#CC := icc
#LINKER := icc
CC := gcc
CXX := g++
LINKER := g++
LIBLINKER := g++

#arch := $(shell uname -m)

os := $(shell uname -s)
ifeq ($(strip $(os)),Darwin)
  macosx := true
else
  macosx := false
endif

#debug := false
debug := true

#DO NOT SHIP WITH PROFILING ENABLED!
# You will probably need to set static_link := true below, too.
# (This seems to be broken right now. Thanks, gcc.)
profile := false
#profile := true

# DO NOT SHIP WITH THIS!
# In fact, you almost should never set this to true.
# (What a surprise, this doesn't work either. Thanks, gcc.)
soft_float := false
#soft_float := true

#use_mmx_intrinsics := true
use_mmx_intrinsics := false


#use_efence := true
use_efence := false
EFENCELIB := /usr/lib/libefence.a

USE_ASM := -DUSE_I386_ASM
#USE_ASM := -DUSE_PORTABLE_C

# Build everything as a single-threaded program.
#  Note that SDL and other libraries may spin threads of their own, but
#  generally it's safe to assume those threads are independent of the game
#  and won't cause race conditions. The ones that do, like the SDL timer
#  thread, are taken into account with this option.
#
# In multithreaded mode, The Serious Engine and SeriousSam only spin one
#  thread: the timer thread, which runs a "todo list" every 1/20 seconds.
#  Single threaded mode makes periodic calls to a function that determines if
#  it is time to run the todo list, and immediately returns if not. In such a
#  mode, we don't have to concern ourselves with race conditions, mutex
#  contention, and thread local storage, but could have serious problems
#  ("queue lag") if the timer-checking function doesn't get called regularly.
#  Lag isn't a concern in-game, since the main loop has such a call, but it
#  isn't hard to get into a blocking loop (map loading, etc) that causes a
#  queue lag. Generally, these just need to be flushed out, but if they
#  haven't been...well, You Have Been Warned.
single_threaded := true
#single_threaded := false

# (You should probably should leave this set to true.)
netstructs_packed := true
#netstructs_packed := false

# Set this to statically link the main binary with the game and entities
#  objects. Without this, game and entities are separate shared libraries that
#  are dlopen()'d at runtime. Enabling this is just for debugging purposes,
#  mostly. SDL and other LGPL'd code is statically linked in the main binary
#  regardless of this option (a binaryname.dynamic version is also built to
#  satisfy LGPL requirements, again, regardless of this option).
#static_link := $(debug)
#static_link := true
static_link := false


# Set this to true to use the MSVC inline assembly with the intel compiler.
#  (Setting is ignored when building with something other than "icc".)
use_msvc_asm := true
#use_msvc_asm := false


# Set this to make a binary that refuses to run 30 days after it was compiled.
#  (after SeriousSam/SeriousSam.cpp was compiled, specifically.)
#expiring_beta := $(debug)
#expiring_beta := true
expiring_beta := false


# Use this to automate the moving of binaries and updated source to another
#  system for remote debugging. USE WITH CAUTION, since source and
#  unofficial binaries will be going over a wire. Setting permit_scp to false
#  completely removes the "make scp" rule from the Makefile at build time.
permit_scp := true
scp_bin := scp
scp_user := icculus
scp_host := liza
scp_dir := Croteam/ssam/Bin
scp_src_dir := projects/ssam

BINDIR := bin
SRCDIR := .

# SDL directory for static linking.
SDLDIR := /usr/local/lib

INSTALLDIR := $(HOME)/Croteam/ssam/Bin
ifeq ($(strip $(debug)),true)
  INSTALLDIR := $(strip $(INSTALLDIR))/Debug
  scp_dir := $(strip $(scp_dir))/Debug
endif

# You need the Netwide Assembler (NASM) to build this on Intel systems.
#   http://nasm.sf.net/
# Intel Mac OS X needs at least nasm 0.98.40, which is CVS-only at the
#  time of this writing, since it adds x86 Mach-O object file support.
ASM := nasm

ifeq ($(strip $(macosx)),true)
  ASMOBJFMT := macho
  ASMFLAGS += --prefix _
else
  ASMOBJFMT := elf
endif

EXE_EXT :=

#-----------------------------------------------------------------------------#
# Rules.
#-----------------------------------------------------------------------------#
CLEANUP = $(wildcard *.exe) $(wildcard *.obj) \
          $(wildcard $(BINDIR)/*.exe) $(wildcard $(BINDIR)/*.obj) \
          $(wildcard *~) $(wildcard *.err) \
          $(wildcard .\#*) core

ENGINEBASESRCS := Engine/Base/Anim.cpp Engine/Base/CRC.cpp \
                  Engine/Base/CRCTable.cpp Engine/Base/Changeable.cpp \
                  Engine/Base/Console.cpp Engine/Base/Directory.cpp \
                  Engine/Base/ErrorReporting.cpp Engine/Base/FileName.cpp \
                  Engine/Base/Input.cpp Engine/Base/Lists.cpp \
                  Engine/Base/Memory.cpp Engine/Base/Profiling.cpp \
                  Engine/Base/ProgressHook.cpp Engine/Base/Protection.cpp \
                  Engine/Base/Relations.cpp \
                  Engine/Base/ReplaceFile.cpp Engine/Base/Serial.cpp \
                  Engine/Base/Shell.cpp Engine/Base/ShellTypes.cpp \
                  Engine/Base/Statistics.cpp \
                  Engine/Base/Stream.cpp \
                  Engine/Base/Timer.cpp Engine/Base/Translation.cpp \
                  Engine/Base/Unzip.cpp Engine/Base/Updateable.cpp \
                  Engine/Base/CTString.cpp \
                  Engine/Base/Scanner.cpp \
                  Engine/Base/Parser.cpp \
                  Engine/Base/IFeel.cpp \
                  Engine/Base/Unix/UnixFileSystem.cpp \
                  Engine/Base/Unix/UnixDynamicLoader.cpp \
                  Engine/Base/SDL/SDLTimer.cpp \
                  Engine/Base/SDL/SDLInput.cpp \
                  Engine/Base/SDL/SDLEvents.cpp

ifeq ($(strip $(single_threaded)),true)
  ENGINEBASESRCS += Engine/Base/NullSynchronization.cpp
else
  ENGINEBASESRCS += Engine/Base/Unix/UnixSynchronization.cpp
  ENGINEBASESRCS += Engine/Base/SDL/SDLThreadLocalStorage.cpp
endif

# Do we need this? How reliant on the registry is the engine? We could fake
#  these calls easily enough if need be.
#ENGINEBASESRCS += Engine/Base/Registry.cpp

# Stuff that probably shouldn't be compiled at all...
#ENGINEBASESRCS += Engine/Base/StackDump.cpp

# Engine/Base/Synchronization.cpp is now Engine/Base/SDL/SDLSynchronization.cpp
#  in Linux, and Engine/Base/Win32/Win32Synchronization.cpp for Windows.

ENGINEBRUSHESSRCS := Engine/Brushes/Brush.cpp Engine/Brushes/BrushIO.cpp \
                     Engine/Brushes/BrushShadows.cpp \
                     Engine/Brushes/BrushTriangularize.cpp \
                     Engine/Brushes/BrushArchive.cpp \
                     Engine/Brushes/BrushImport.cpp \
                     Engine/Brushes/BrushMip.cpp \
                     Engine/Brushes/BrushPolygon.cpp \
                     Engine/Brushes/BrushExport.cpp \
                     Engine/Brushes/BrushSector.cpp

ENGINEENTITYSRCS := Engine/Entities/Entity.cpp \
                    Engine/Entities/NearestPolygon.cpp \
                    Engine/Entities/EntityProperties.cpp \
                    Engine/Entities/PlayerCharacter.cpp \
                    Engine/Entities/EntityClass.cpp \
                    Engine/Entities/FieldBSPTesting.cpp \
                    Engine/Entities/EntityCollision.cpp \
                    Engine/Entities/EntityCopying.cpp \
                    Engine/Entities/LastPositions.cpp

ENGINEMATHSRCS := Engine/Math/Projection_Isometric.cpp \
                  Engine/Math/Object3D.cpp \
                  Engine/Math/Projection_Parallel.cpp \
                  Engine/Math/Projection_Perspective.cpp \
                  Engine/Math/Float.cpp \
                  Engine/Math/Object3D_CSG.cpp \
                  Engine/Math/Projection_Simple.cpp \
                  Engine/Math/Projection_Simple_DOUBLE.cpp \
                  Engine/Math/Functions.cpp \
                  Engine/Math/ObjectSector.cpp \
                  Engine/Math/Placement.cpp \
                  Engine/Math/TextureMapping.cpp \
                  Engine/Math/Geometry.cpp \
                  Engine/Math/Projection.cpp \
                  Engine/Math/Geometry_DOUBLE.cpp

# This uses that "Exploration 3D library. Hopefully we can just skip this file.
#ENGINEMATHSRCS += Engine/Math/Object3D_IO.cpp

ENGINEMODELSSRCS := Engine/Models/Model.cpp \
                    Engine/Models/RenderModel_View.cpp \
                    Engine/Models/Normals.cpp \
                    Engine/Models/VertexGetting.cpp \
                    Engine/Models/RenderModel.cpp \
                    Engine/Models/MipMaker.cpp \
                    Engine/Models/ModelProfile.cpp \
                    Engine/Models/RenderModel_Mask.cpp

# I don't think this is needed for the client or server, and it needs symbols
#  from Object3D_IO.cpp, which needs stuff from that win32 library...
#ENGINEMODELSSRCS += Engine/Models/EditModel.cpp


ENGINELIGHTSRCS := Engine/Light/LayerMaker.cpp \
                   Engine/Light/LayerMixer.cpp \
                   Engine/Light/LightSource.cpp

ENGINEGRAPHICSSRCS := Engine/Graphics/Adapter.cpp \
                      Engine/Graphics/Raster.cpp \
                      Engine/Graphics/GfxLibrary.cpp \
                      Engine/Graphics/Benchmark.cpp \
                      Engine/Graphics/GfxProfile.cpp \
                      Engine/Graphics/Color.cpp \
                      Engine/Graphics/ShadowMap.cpp \
                      Engine/Graphics/DepthCheck.cpp \
                      Engine/Graphics/Texture.cpp \
                      Engine/Graphics/DisplayMode.cpp \
                      Engine/Graphics/Gfx_OpenGL.cpp \
                      Engine/Graphics/Gfx_OpenGL_Textures.cpp \
                      Engine/Graphics/TextureEffects.cpp \
                      Engine/Graphics/DrawPort.cpp \
                      Engine/Graphics/Gfx_wrapper.cpp \
                      Engine/Graphics/DrawPort_Particles.cpp \
                      Engine/Graphics/Graphics.cpp \
                      Engine/Graphics/ViewPort.cpp \
                      Engine/Graphics/DrawPort_RenderScene.cpp \
                      Engine/Graphics/ImageInfo.cpp \
                      Engine/Graphics/Fog.cpp \
                      Engine/Graphics/MultiMonitor.cpp \
                      Engine/Graphics/Font.cpp \
                      Engine/Graphics/Shader.cpp \
                      Engine/Graphics/Stereo.cpp \
                      Engine/Graphics/SDL/SDLOpenGL.cpp \
                      Engine/Graphics/SDL/SDLAdapter.cpp

ENGINENETWORKSRCS := Engine/Network/ActionBuffer.cpp \
                     Engine/Network/NetworkMessage.cpp \
                     Engine/Network/Server.cpp \
                     Engine/Network/GameSpy.cpp \
                     Engine/Network/Buffer.cpp \
                     Engine/Network/NetworkProfile.cpp \
                     Engine/Network/GameSpyEnum.cpp \
                     Engine/Network/SessionState.cpp \
                     Engine/Network/GameSpyKeys.cpp \
                     Engine/Network/PlayerBuffer.cpp \
                     Engine/Network/MessageDispatcher.cpp \
                     Engine/Network/PlayerSource.cpp \
                     Engine/Network/Compression.cpp \
                     Engine/Network/Network.cpp \
                     Engine/Network/PlayerTarget.cpp \
                     Engine/Network/CPacket.cpp \
                     Engine/Network/ClientInterface.cpp \
                     Engine/Network/CommunicationInterface.cpp \
                     Engine/Network/Diff.cpp


ENGINEGAMESPYSRCS := Engine/gamespy/darray.c \
                     Engine/gamespy/gqueryreporting.c \
                     Engine/gamespy/gserver.c \
                     Engine/gamespy/gserverlist.c \
                     Engine/gamespy/hashtable.c \
                     Engine/gamespy/nonport.c

ENGINETERRAINSRCS := Engine/Terrain/ArrayHolder.cpp \
                     Engine/Terrain/Terrain.cpp \
                     Engine/Terrain/TerrainArchive.cpp \
                     Engine/Terrain/TerrainEditing.cpp \
                     Engine/Terrain/TerrainLayer.cpp \
                     Engine/Terrain/TerrainMisc.cpp \
                     Engine/Terrain/TerrainRayCasting.cpp \
                     Engine/Terrain/TerrainRender.cpp \
                     Engine/Terrain/TerrainTile.cpp


# this code is included in Engine/gamespy/gqueryreporting, too...
#ENGINEGAMESPYSRCS += Engine/gamespy/gutil.c

ENGINERENDERINGSRCS := Engine/Rendering/Render.cpp \
                       Engine/Rendering/RenderProfile.cpp \
                       Engine/Rendering/SelectOnRender.cpp

ENGINESKASRCS := Engine/Ska/AnimSet.cpp \
                 Engine/Ska/RMRender.cpp \
                 Engine/Ska/Skeleton.cpp \
                 Engine/Ska/ModelInstance.cpp \
                 Engine/Ska/StringTable.cpp \
                 Engine/Ska/Mesh.cpp \
                 Engine/Ska/RMRenderMask.cpp \
                 Engine/Ska/smcPars.cpp \
                 Engine/Ska/smcScan.cpp

ENGINESOUNDSRCS := Engine/Sound/SoundDecoder.cpp \
                   Engine/Sound/SoundObject.cpp \
                   Engine/Sound/SoundLibrary.cpp \
                   Engine/Sound/SoundProfile.cpp \
                   Engine/Sound/SoundData.cpp \
                   Engine/Sound/Wave.cpp \
                   Engine/Sound/SoundMixer.cpp

ifeq ($(strip $(USE_ASM)),-DUSE_I386_ASM)
  ENGINESOUNDSRCS += Engine/Sound/SoundMixer386.asm
endif

ENGINETEMPLATESSRCS := Engine/Templates/Stock_CAnimData.cpp \
                       Engine/Templates/Stock_CAnimSet.cpp \
                       Engine/Templates/Stock_CEntityClass.cpp \
                       Engine/Templates/Stock_CMesh.cpp \
                       Engine/Templates/Stock_CModelData.cpp \
                       Engine/Templates/Stock_CSkeleton.cpp \
                       Engine/Templates/Stock_CSoundData.cpp \
                       Engine/Templates/Stock_CTextureData.cpp \
                       Engine/Templates/Stock_CShader.cpp \
                       Engine/Templates/NameTable_CTFileName.cpp \
                       Engine/Templates/NameTable_CTranslationPair.cpp \
                       Engine/Templates/BSP.cpp

ENGINEWORLDSRCS := Engine/World/WorldCSG.cpp \
                   Engine/World/PhysicsProfile.cpp \
                   Engine/World/WorldCollision.cpp \
                   Engine/World/WorldIO.cpp \
                   Engine/World/WorldRayCasting.cpp \
                   Engine/World/World.cpp \
                   Engine/World/WorldCollisionGrid.cpp \
                   Engine/World/WorldEditingProfile.cpp

ENGINEZLIBSRCS := Engine/zlib/inflate.c \
                  Engine/zlib/adler32.c \
                  Engine/zlib/infblock.c \
                  Engine/zlib/inffast.c \
                  Engine/zlib/inftrees.c \
                  Engine/zlib/trees.c \
                  Engine/zlib/compress.c \
                  Engine/zlib/zutil.c \
                  Engine/zlib/deflate.c \
                  Engine/zlib/infcodes.c \
                  Engine/zlib/infutil.c \
                  Engine/zlib/uncompr.c


ENGINESRCS := Engine/Engine.cpp \
              $(ENGINEBASESRCS) \
              $(ENGINEBRUSHESSRCS) \
              $(ENGINEENTITYSRCS) \
              $(ENGINEGRAPHICSSRCS) \
              $(ENGINELIGHTSRCS) \
              $(ENGINEMATHSRCS) \
              $(ENGINEMODELSSRCS) \
              $(ENGINENETWORKSRCS) \
              $(ENGINETERRAINSRCS) \
              $(ENGINERENDERINGSRCS) \
              $(ENGINESKASRCS) \
              $(ENGINESOUNDSRCS) \
              $(ENGINETEMPLATESSRCS) \
              $(ENGINEWORLDSRCS) \
              $(ENGINEGAMESPYSRCS) \
              $(ENGINEZLIBSRCS)

OBJS1 := $(ENGINESRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
ENGINEOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
ENGINESRCS := $(foreach f,$(ENGINESRCS),$(SRCDIR)/$(f))

SHADERSSRCS := Shaders/AddShader.cpp \
               Shaders/AddShaderDS.cpp \
               Shaders/BaseShader.cpp \
               Shaders/BaseShaderDS.cpp \
               Shaders/BaseTransparent.cpp \
               Shaders/BaseTransparentDS.cpp \
               Shaders/ColorShader.cpp \
               Shaders/Common.cpp \
               Shaders/DetailShader.cpp \
               Shaders/DisplaceShader.cpp \
               Shaders/InvisibleShader.cpp \
               Shaders/MultiLayerShader.cpp \
               Shaders/Reflection.cpp \
               Shaders/ReflectionDS.cpp \
               Shaders/ReftectionAndSpecular.cpp \
               Shaders/ReftectionAndSpecularDS.cpp \
               Shaders/Specular.cpp \
               Shaders/SpecularDS.cpp \
               Shaders/StdH.cpp \
               Shaders/Translucent.cpp

OBJS1 := $(SHADERSSRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
SHADERSOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
SHADERSSRCS := $(foreach f,$(SHADERSSRCS),$(SRCDIR)/$(f))


GAMESRCS := Game/Camera.cpp Game/CompMessage.cpp Game/CompModels.cpp \
            Game/Computer.cpp Game/Console.cpp Game/Controls.cpp \
            Game/Game.cpp Game/LCDDrawing.cpp Game/LoadingHook.cpp \
            Game/Map.cpp Game/SessionProperties.cpp Game/WEDInterface.cpp

OBJS1 := $(GAMESRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
GAMEOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
GAMESRCS := $(foreach f,$(GAMESRCS),$(SRCDIR)/$(f))


GAMEMPSRCS := GameMP/Camera.cpp GameMP/CompMessage.cpp GameMP/CompModels.cpp \
              GameMP/Computer.cpp GameMP/Console.cpp GameMP/Controls.cpp \
              GameMP/Game.cpp GameMP/LCDDrawing.cpp GameMP/LoadingHook.cpp \
              GameMP/Map.cpp GameMP/SessionProperties.cpp GameMP/WEDInterface.cpp

OBJS1 := $(GAMEMPSRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
GAMEMPOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
GAMEMPSRCS := $(foreach f,$(GAMEMPSRCS),$(SRCDIR)/$(f))

ENTITYFILES := $(wildcard Entities/*.es)
ENTITYHEADERS := $(ENTITYFILES:.es=.h)
ENTITYTABLES := $(ENTITYFILES:.es=_tables.h)
ENTITYSRCS := $(ENTITYHEADERS:.h=.cpp)

ENTITYCOMMONSRCS := Entities/Common/Common.cpp Entities/Common/Particles.cpp \
                    Entities/Common/PathFinding.cpp Entities/Common/HUD.cpp

# These file are empty right now. ?
#ENTITYCOMMONSRCS += Entities/Common/Stats.cpp
#ENTITYCOMMONSRCS += Entities/Common/Debris.cpp

ENTITYHEADERS := $(foreach f,$(ENTITYHEADERS),$(SRCDIR)/$(f))

OBJS1 := $(ENTITYSRCS:.cpp=.o) $(ENTITYCOMMONSRCS:.cpp=.o)
ENTITYOBJS := $(foreach f,$(OBJS1),$(BINDIR)/$(f))
ENTITYSRCS := $(foreach f,$(ENTITYSRCS),$(SRCDIR)/$(f))


ENTITYMPFILES := $(wildcard EntitiesMP/*.es)
ENTITYMPHEADERS := $(ENTITYMPFILES:.es=.h)
ENTITYMPTABLES := $(ENTITYMPFILES:.es=_tables.h)
ENTITYMPSRCS := $(ENTITYMPHEADERS:.h=.cpp)

ENTITYMPCOMMONSRCS := EntitiesMP/Common/Common.cpp \
					  EntitiesMP/Common/Particles.cpp \
					  EntitiesMP/Common/EmanatingParticles.cpp \
					  EntitiesMP/Common/PathFinding.cpp \
					  EntitiesMP/Common/HUD.cpp

# These file are empty right now. ?
#ENTITYMPCOMMONSRCS += EntitiesMP/Common/Stats.cpp
#ENTITYMPCOMMONSRCS += EntitiesMP/Common/Debris.cpp

ENTITYMPHEADERS := $(foreach f,$(ENTITYMPHEADERS),$(SRCDIR)/$(f))

OBJS1 := $(ENTITYMPSRCS:.cpp=.o) $(ENTITYMPCOMMONSRCS:.cpp=.o)
ENTITYMPOBJS := $(foreach f,$(OBJS1),$(BINDIR)/$(f))
ENTITYMPSRCS := $(foreach f,$(ENTITYMPSRCS),$(SRCDIR)/$(f))

# These are done separately, after ENTITYOBJS is initialized, because it
#  should only be linked into the main binary.
ENGINEENTITYFILES := $(wildcard Engine/Classes/*.es)
ENGINEENTITYHEADERS := $(ENGINEENTITYFILES:.es=.h)
ENGINEENTITYTABLES := $(ENGINEENTITYFILES:.es=_tables.h)
ENGINEENTITYSRCS := $(ENGINEENTITYHEADERS:.h=.cpp)
OBJS1 := $(ENGINEENTITYSRCS:.cpp=.o)
ENGINEENTITYOBJS := $(foreach f,$(OBJS1),$(BINDIR)/$(f))
ENGINEENTITYSRCS := $(foreach f,$(ENGINEENTITYSRCS),$(SRCDIR)/$(f))

ENTITYFILES += $(ENGINEENTITYFILES)
ENTITYHEADERS += $(ENGINEENTITYHEADERS)
ENTITYTABLES += $(ENGINEENTITYTABLES)
ENTITYSRCS += $(ENGINEENTITYSRCS)

ENTITYMPFILES += $(ENGINEENTITYFILES)
ENTITYMPHEADERS += $(ENGINEENTITYHEADERS)
ENTITYMPTABLES += $(ENGINEENTITYTABLES)
ENTITYMPSRCS += $(ENGINEENTITYSRCS)

ifeq ($(strip $(macosx)),true)
  DLLEXT := dylib
else
  DLLEXT := so
endif
ifeq ($(strip $(debug)),true)
  ENTITYDLL := $(BINDIR)/libEntitiesD.$(DLLEXT)
  ENTITYMPDLL := $(BINDIR)/libEntitiesMPD.$(DLLEXT)
  GAMEDLL := $(BINDIR)/libGameD.$(DLLEXT)
  GAMEMPDLL := $(BINDIR)/libGameMPD.$(DLLEXT)
  SHADERSDLL := $(BINDIR)/libShadersD.$(DLLEXT)
else
  ENTITYDLL := $(BINDIR)/libEntities.$(DLLEXT)
  ENTITYMPDLL := $(BINDIR)/libEntitiesMP.$(DLLEXT)
  GAMEDLL := $(BINDIR)/libGame.$(DLLEXT)
  GAMEMPDLL := $(BINDIR)/libGameMP.$(DLLEXT)
  SHADERSDLL := $(BINDIR)/libShaders.$(DLLEXT)
endif

ECCEXE := $(BINDIR)/ecc-bin
ECCSRCS := Ecc/Main.cpp Ecc/Parser.cpp Ecc/Scanner.cpp
OBJS1 := $(ECCSRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
ECCOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
ECCSRCS := $(foreach f,$(ECCSRCS),$(SRCDIR)/$(f))

DEDICATEDEXE := $(BINDIR)/ssamded-bin.static
DEDICATEDEXE_DYNAMIC := $(BINDIR)/ssamded-bin
DEDICATEDSRCS := DedicatedServer/DedicatedServer.cpp
OBJS1 := $(DEDICATEDSRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
DEDICATEDOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
DEDICATEDSRCS := $(foreach f,$(DEDICATEDSRCS),$(SRCDIR)/$(f))

SERIOUSSAMEXE := $(BINDIR)/ssam-bin.static
SERIOUSSAMEXE_DYNAMIC := $(BINDIR)/ssam-bin
SERIOUSSAMSRCS := SeriousSam/LevelInfo.cpp SeriousSam/CmdLine.cpp \
                  SeriousSam/SeriousSam.cpp SeriousSam/VarList.cpp \
                  SeriousSam/Credits.cpp SeriousSam/Menu.cpp \
                  SeriousSam/GLSettings.cpp SeriousSam/MenuGadgets.cpp \
                  SeriousSam/LCDDrawing.cpp SeriousSam/MenuPrinting.cpp \
                  SeriousSam/SplashScreen.cpp SeriousSam/MainWindow.cpp

OBJS1 := $(SERIOUSSAMSRCS:.c=.o)
OBJS2 := $(OBJS1:.cpp=.o)
OBJS3 := $(OBJS2:.asm=.o)
SERIOUSSAMOBJS := $(foreach f,$(OBJS3),$(BINDIR)/$(f))
SERIOUSSAMSRCS := $(foreach f,$(SERIOUSSAMSRCS),$(SRCDIR)/$(f))

ifeq ($(strip $(static_link)),true)
DEDICATEDEXTRAOBJS := $(ENTITYOBJS) $(GAMEOBJS)
SERIOUSSAMEXTRAOBJS := $(ENTITYOBJS) $(GAMEOBJS) $(SHADERSOBJS)
CFLAGS += -DSTATICALLY_LINKED
endif

#INSTALLABLES := $(DEDICATEDEXE) $(SERIOUSSAMEXE) $(DEDICATEDEXE_DYNAMIC) $(SERIOUSSAMEXE_DYNAMIC) $(ECCEXE)
#INSTALLABLES := $(SERIOUSSAMEXE)
INSTALLABLES := $(SERIOUSSAMEXE_DYNAMIC) $(DEDICATEDEXE_DYNAMIC)
ifneq ($(strip $(static_link)),true)
  INSTALLABLES += $(ENTITYDLL) $(ENTITYMPDLL) $(GAMEDLL) $(GAMEMPDLL) $(SHADERSDLL)
endif

# !!! FIXME: Get -Wall in here, some day.
CFLAGS += -IExternal/SDL12
CFLAGS += $(USE_ASM) -I$(SRCDIR) -D_REENTRANT -DPLATFORM_UNIX -D_MT
ifeq ($(strip $(macosx)),true)
  CFLAGS += -DPLATFORM_MACOSX
  CXXFLAGS += -Wno-invalid-offsetof
endif

ifeq ($(strip $(CC)),icc)
  # intel c++ options...
  CFLAGS += $(USE_ASM) -I$(SRCDIR) -I/opt/intel/compiler60/ia32/include -D_REENTRANT -DPLATFORM_UNIX -D_MT -fp -Kc++eh -Krtti

  ifeq ($(strip $(use_msvc_asm)),true)
    CFLAGS += -use_msasm -D__MSVC_INLINE__
  else
    CFLAGS += -D__GNU_INLINE__
  endif

else
  # gcc options...
  # !!! FIXME: Get -Wall in here, some day.
  # !!! FIXME: Get -Werror back in here for gcc3, some day.
  CFLAGS += $(USE_ASM) -I$(SRCDIR) -D_REENTRANT -fsigned-char -DPLATFORM_UNIX
  CXXFLAGS += -Wno-non-template-friend -fexceptions -frtti
  CFLAGS += -D_MT -fno-omit-frame-pointer
endif

ifeq ($(strip $(soft_float)),true)
  CFLAGS += -msoft-float
  LDFLAGS += -msoft-float
endif

ifeq ($(strip $(single_threaded)),true)
  CFLAGS += -DSINGLE_THREADED
endif

ifeq ($(strip $(netstructs_packed)),true)
  CFLAGS += -DNETSTRUCTS_PACKED
endif

ifeq ($(strip $(expiring_beta)),true)
  CFLAGS += -DBETAEXPIRE=$(shell date +%s)
endif

ifeq ($(strip $(profile)),true)
  CFLAGS += -DPROFILING_ENABLED -pg
endif

ifeq ($(strip $(use_efence)),false)
  EFENCELIB :=
endif


NOINLINE := -O0

# (you can always strip the binaries later...)
CFLAGS += -g
ASMFLAGS += -g
LDFLAGS += -g

ifeq ($(strip $(debug)),true)
  CFLAGS += -DDEBUG -O0
else
  ifeq ($(strip $(CC)),icc)
    OPTFLAG := -O3
    CFLAGS += -xMi -tpp6 -rcd
  else
    #OPTFLAG := -O0
    #OPTFLAG := -O2
    OPTFLAG := -O3 -ffast-math -falign-loops=16 -fno-math-errno
    ifneq ($(strip $(macosx)),true)
      OPTFLAG += -march=pentium -mcpu=pentiumpro
    else
      ifeq ($(strip $(static_link)),true)
      	OPTFLAG += -mdynamic-no-pic
      endif
    endif
  endif
  CFLAGS += -DNDEBUG -D_NDEBUG $(OPTFLAG)
endif

ifeq ($(strip $(CC)),icc)
  PICFLAG := -KPIC
else
  PICFLAG := -fPIC
endif
DLLLDFLAGS := $(PICFLAG) $(LDFLAGS)
ifeq ($(strip $(macosx)),true)
  DLLLDFLAGS += -dynamiclib -undefined dynamic_lookup
else
  DLLLDFLAGS += -shared
endif

#STATIC_SDL_LDFLAGS := -L/usr/X11R6/lib $(SDLDIR)/libSDL.a /usr/lib/libasound.a /usr/X11R6/lib/libxme.a /usr/local/lib/libesd.a -lpthread -lX11 -lXext -lXxf86dga -lXxf86vm -lXv -lXinerama
#-Wl,-Bstatic $(SDLDIR)/libSDL.a /usr/lib/libesd.a -Wl,-Bdynamic -lpthread -lX11 -lXext -lXxf86dga -lXxf86vm -lXv -lXinerama

#DYNAMIC_SDL_LDFLAGS := $(shell sdl-config --libs)
DYNAMIC_SDL_LDFLAGS :=
ifeq ($(strip $(macosx)),true)
  DYNAMIC_SDL_LDFLAGS := lib/macosx_x86/libSDL-1.2.0.dylib bin/libSDLmain.a
else
  DYNAMIC_SDL_LDFLAGS := lib/linux_x86/libSDL-1.2.so.0
endif

ifeq ($(strip $(macosx)),true)
  LDFLAGS := -framework Cocoa -framework OpenGL
else
  LDFLAGS += -rdynamic -lm -ldl
endif

CFLAGS += $(shell sdl-config --cflags)

ifneq ($(strip $(USE_ASM)),-DUSE_PORTABLE_C)
#  CFLAGS += -O2 -fasm
  ASMFLAGS += -f $(ASMOBJFMT) $(ASMDEFS)
else
endif

ifeq ($(strip $(use_mmx_intrinsics)),true)
  CFLAGS += -mmmx -DUSE_MMX_INTRINSICS=1
endif

CXXFLAGS += $(CFLAGS)


# Entity rules...
%.h : %.es $(ECCEXE)
	$(ECCEXE) $<
	@touch $@

$(BINDIR)/Entities/%.o: $(SRCDIR)/Entities/%.cpp
	$(CXX) -c -o $@ $< -I$(dir $<)StdH $(CXXFLAGS)

$(BINDIR)/EntitiesMP/%.o: $(SRCDIR)/EntitiesMP/%.cpp
	$(CXX) -c -o $@ $< -I$(dir $<)StdH $(CXXFLAGS)

$(BINDIR)/Engine/Classes/%.o: $(SRCDIR)/Engine/Classes/%.cpp
	$(CXX) -c -o $@ $< -I$(dir $<)StdH $(CXXFLAGS)


# Rules for turning source files into .o files

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

.PHONY: all dedicated clean distclean ecc entities entitiesmp gamedll gamempdll install ssam

all: $(BINDIR) ecc ssam dedicated

#dedicated: $(BINDIR) $(DEDICATEDEXE) $(DEDICATEDEXE_DYNAMIC)
dedicated: $(BINDIR) $(DEDICATEDEXE_DYNAMIC)
ecc: $(BINDIR) $(ECCEXE)
#ssam: $(BINDIR) $(SERIOUSSAMEXE) $(SERIOUSSAMEXE_DYNAMIC)
ssam: $(BINDIR) $(SERIOUSSAMEXE_DYNAMIC)
entities: ecc $(ENTITYHEADERS)
entitiesmp: ecc $(ENTITYMPHEADERS)
gamedll: $(BINDIR) $(GAMEDLL)
gamempdll: $(BINDIR) $(GAMEMPDLL)

install: all
	cp $(INSTALLABLES) $(INSTALLDIR)

ifeq ($(strip $(permit_scp)),true)
scp:
	@scripts/scp.pl $(scp_user) $(scp_host) $(scp_src_dir) $(scp_bin)
	$(scp_bin) $(INSTALLABLES) $(strip $(scp_user))@$(strip $(scp_host)):$(strip $(scp_dir))
endif

ifeq ($(strip $(macosx)),true)
SDLMAINLIB := bin/libSDLmain.a
$(SDLMAINLIB) : lib/macosx_x86/libSDLmain.a $(BINDIR)
	cp $< $@
	ranlib $@
endif

$(ENTITYDLL): $(ENTITYHEADERS) $(ENTITYOBJS)
	$(LIBLINKER) $(DLLLDFLAGS) -o $(ENTITYDLL) $(ENTITYOBJS) $(EFENCELIB)

$(ENTITYMPDLL): $(ENTITYMPHEADERS) $(ENTITYMPOBJS)
	$(LIBLINKER) $(DLLLDFLAGS) -o $(ENTITYMPDLL) $(ENTITYMPOBJS) $(EFENCELIB)

$(GAMEDLL): $(GAMEOBJS)
	$(LIBLINKER) $(DLLLDFLAGS) -o $(GAMEDLL) $(GAMEOBJS) $(EFENCELIB)

$(GAMEMPDLL): $(GAMEMPOBJS)
	$(LIBLINKER) $(DLLLDFLAGS) -o $(GAMEMPDLL) $(GAMEMPOBJS) $(EFENCELIB)

$(SHADERSDLL): $(SHADERSOBJS)
	$(LIBLINKER) $(DLLLDFLAGS) -o $(SHADERSDLL) $(SHADERSOBJS) $(EFENCELIB)

$(SRCDIR)/Ecc/Scanner.cpp: $(SRCDIR)/Ecc/Scanner.l $(SRCDIR)/Ecc/Parser.cpp
	flex -o$(SRCDIR)/Ecc/Scanner.cpp $(SRCDIR)/Ecc/Scanner.l

$(SRCDIR)/Ecc/Parser.cpp: $(SRCDIR)/Ecc/Parser.y
	bison -o$(SRCDIR)/Ecc/Parser.cpp $(SRCDIR)/Ecc/Parser.y -d
	if [ -f $(SRCDIR)/Ecc/Parser.hpp ]; then mv $(SRCDIR)/Ecc/Parser.hpp $(SRCDIR)/Ecc/Parser.h ; fi
	if [ -f $(SRCDIR)/Ecc/Parser.cpp.h ]; then mv $(SRCDIR)/Ecc/Parser.cpp.h $(SRCDIR)/Ecc/Parser.h ; fi

$(SRCDIR)/Engine/Base/Scanner.cpp: $(SRCDIR)/Engine/Base/Scanner.l $(SRCDIR)/Engine/Base/Parser.cpp
	flex -o$(SRCDIR)/Engine/Base/Scanner.cpp $(SRCDIR)/Engine/Base/Scanner.l

$(SRCDIR)/Engine/Base/Parser.cpp: $(SRCDIR)/Engine/Base/Parser.y
	bison -o$(SRCDIR)/Engine/Base/Parser.cpp $(SRCDIR)/Engine/Base/Parser.y -d
	if [ -f $(SRCDIR)/Engine/Base/Parser.hpp ]; then mv $(SRCDIR)/Engine/Base/Parser.hpp $(SRCDIR)/Engine/Base/Parser.h ; fi
	if [ -f $(SRCDIR)/Engine/Base/Parser.cpp.h ]; then mv $(SRCDIR)/Engine/Base/Parser.cpp.h $(SRCDIR)/Engine/Base/Parser.h ; fi

$(SRCDIR)/Engine/Ska/smcScan.cpp: $(SRCDIR)/Engine/Ska/smcScan.l $(SRCDIR)/Engine/Ska/smcPars.cpp
	flex -o$(SRCDIR)/Engine/Ska/smcScan.cpp $(SRCDIR)/Engine/Ska/smcScan.l

$(SRCDIR)/Engine/Ska/smcPars.cpp: $(SRCDIR)/Engine/Ska/smcPars.y
	bison -o$(SRCDIR)/Engine/Ska/smcPars.cpp $(SRCDIR)/Engine/Ska/smcPars.y -d
	if [ -f $(SRCDIR)/Engine/Ska/smcPars.hpp ]; then mv $(SRCDIR)/Engine/Ska/smcPars.hpp $(SRCDIR)/Engine/Ska/smcPars.h ; fi
	if [ -f $(SRCDIR)/Engine/Ska/smcPars.cpp.h ]; then mv $(SRCDIR)/Engine/Ska/smcPars.cpp.h $(SRCDIR)/Engine/Ska/smcPars.h ; fi

ifeq ($(strip $(static_link)),true)
  ENTITYDLL :=
  ENTITYMPDLL :=
  GAMEDLL :=
  GAMEMPDLL :=
  SHADERSDLL :=
endif

$(ECCEXE): $(SRCDIR)/Ecc/Scanner.cpp $(SRCDIR)/Ecc/Parser.cpp $(ECCOBJS)
	$(LINKER) $(LDFLAGS) -o $(ECCEXE) $(ECCOBJS) #$(EFENCELIB)

$(DEDICATEDEXE): $(ENTITYDLL) $(ENTITYMPDLL) $(GAMEDLL) $(GAMEMPDLL) $(ENGINEENTITYHEADERS) $(ENGINEENTITYOBJS) $(DEDICATEDOBJS) $(ENGINEOBJS) $(DEDICATEDEXTRAOBJS) $(SDLMAINLIB)
	$(LINKER) $(LDFLAGS) -o $(DEDICATEDEXE) $(DEDICATEDOBJS) $(ENGINEOBJS) $(ENGINEENTITYOBJS) $(DEDICATEDEXTRAOBJS) $(EFENCELIB) $(STATIC_SDL_LDFLAGS)

$(DEDICATEDEXE_DYNAMIC): $(ENTITYDLL) $(ENTITYMPDLL) $(GAMEDLL) $(GAMEMPDLL) $(ENGINEENTITYHEADERS) $(ENGINEENTITYOBJS) $(DEDICATEDOBJS) $(ENGINEOBJS) $(DEDICATEDEXTRAOBJS) $(SDLMAINLIB)
	$(LINKER) $(LDFLAGS) -o $(DEDICATEDEXE_DYNAMIC) $(DEDICATEDOBJS) $(ENGINEOBJS) $(ENGINEENTITYOBJS) $(DEDICATEDEXTRAOBJS) $(EFENCELIB) $(DYNAMIC_SDL_LDFLAGS)

$(SERIOUSSAMEXE): $(ENTITYDLL) $(ENTITYMPDLL) $(GAMEDLL) $(GAMEMPDLL) $(SHADERSDLL) $(ENGINEENTITYHEADERS) $(ENGINEENTITYOBJS) $(SERIOUSSAMOBJS) $(ENGINEOBJS) $(SERIOUSSAMEXTRAOBJS) $(SDLMAINLIB)
	$(LINKER) $(LDFLAGS) -o $(SERIOUSSAMEXE) $(SERIOUSSAMOBJS) $(ENGINEOBJS) $(ENGINEENTITYOBJS) $(SERIOUSSAMEXTRAOBJS) $(EFENCELIB) $(STATIC_SDL_LDFLAGS)

$(SERIOUSSAMEXE_DYNAMIC): $(ENTITYDLL) $(ENTITYMPDLL) $(GAMEDLL) $(GAMEMPDLL) $(SHADERSDLL) $(ENGINEENTITYHEADERS) $(ENGINEENTITYOBJS) $(SERIOUSSAMOBJS) $(ENGINEOBJS) $(SERIOUSSAMEXTRAOBJS) $(SDLMAINLIB)
	$(LINKER) $(LDFLAGS) -o $(SERIOUSSAMEXE_DYNAMIC) $(SERIOUSSAMOBJS) $(ENGINEOBJS) $(ENGINEENTITYOBJS) $(SERIOUSSAMEXTRAOBJS) $(EFENCELIB) $(DYNAMIC_SDL_LDFLAGS)

$(BINDIR) :
	mkdir -p $(BINDIR)
	mkdir -p $(BINDIR)/Engine
	mkdir -p $(BINDIR)/Engine/Base
	mkdir -p $(BINDIR)/Engine/Base/Unix
	mkdir -p $(BINDIR)/Engine/Base/SDL
	mkdir -p $(BINDIR)/Engine/Brushes
	mkdir -p $(BINDIR)/Engine/Classes
	mkdir -p $(BINDIR)/Engine/Entities
	mkdir -p $(BINDIR)/Engine/Graphics
	mkdir -p $(BINDIR)/Engine/Graphics/SDL
	mkdir -p $(BINDIR)/Engine/Light
	mkdir -p $(BINDIR)/Engine/Math
	mkdir -p $(BINDIR)/Engine/Models
	mkdir -p $(BINDIR)/Engine/Network
	mkdir -p $(BINDIR)/Engine/Rendering
	mkdir -p $(BINDIR)/Engine/Ska
	mkdir -p $(BINDIR)/Engine/Sound
	mkdir -p $(BINDIR)/Engine/Templates
	mkdir -p $(BINDIR)/Engine/World
	mkdir -p $(BINDIR)/Engine/Terrain
	mkdir -p $(BINDIR)/Engine/gamespy
	mkdir -p $(BINDIR)/Engine/zlib
	mkdir -p $(BINDIR)/DedicatedServer
	mkdir -p $(BINDIR)/Entities
	mkdir -p $(BINDIR)/Entities/Common
	mkdir -p $(BINDIR)/EntitiesMP
	mkdir -p $(BINDIR)/EntitiesMP/Common
	mkdir -p $(BINDIR)/Ecc
	mkdir -p $(BINDIR)/Game
	mkdir -p $(BINDIR)/GameMP
	mkdir -p $(BINDIR)/GameGUI
	mkdir -p $(BINDIR)/GameGUIMP
	mkdir -p $(BINDIR)/Test
	mkdir -p $(BINDIR)/Tutorial
	mkdir -p $(BINDIR)/RCon
	mkdir -p $(BINDIR)/SeriousSam
	mkdir -p $(BINDIR)/Shaders

distclean: clean

clean:
	rm -f $(CLEANUP)
	rm -f $(ENTITYHEADERS)
	rm -f $(ENTITYSRCS)
	rm -f $(ENTITYTABLES)
	rm -f $(ENTITYMPHEADERS)
	rm -f $(ENTITYMPSRCS)
	rm -f $(ENTITYMPTABLES)
	rm -rf $(BINDIR)
	rm -f $(SRCDIR)/Ecc/Scanner.cpp
	rm -f $(SRCDIR)/Ecc/Parser.cpp
	rm -f $(SRCDIR)/Ecc/Parser.h
	rm -f $(SRCDIR)/Engine/Base/Scanner.cpp
	rm -f $(SRCDIR)/Engine/Base/Parser.cpp
	rm -f $(SRCDIR)/Engine/Base/Parser.h
	rm -f $(SRCDIR)/Engine/Ska/smcScan.cpp
	rm -f $(SRCDIR)/Engine/Ska/smcPars.h
	rm -f $(SRCDIR)/Engine/Ska/smcPars.cpp

listentities:
	@echo $(ENTITYFILES)

# end of Makefile ...

