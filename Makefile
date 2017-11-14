################################################################################
# These are variables for the GBA toolchain build
# You can add others if you wish to
# ***** Kevin Berry *****
################################################################################

# The name of your desired GBA game
# This should be a just a name i.e MyFirstGBAGame
# No SPACES AFTER THE NAME.
PROGNAME = Boids

# The object files you want to compile into your program
# This should be a space (SPACE!) separated list of .o files
OFILES = game.o boid.o graphics.o player.o food.o 
OFILES += images/birds.o images/foodimage.o images/majestic_eagle.o
OFILES += text/fontimage.o text/font.o text/text.o text/playerfont.o

# The header files you have created.
# This is necessary to determine when to recompile for files.
# This should be a space (SPACE!) separated list of .h files
HFILES = myLib.h boid.h graphics.h food.h 
HFILES += images/birds.h images/majestic_eagle.h images/foodimage.h
HFILES += text/fontimage.h text/font.h text/text.h text/playerfont.h

################################################################################
# These are various settings used to make the GBA toolchain work
# DO NOT EDIT BELOW.
################################################################################

.PHONY: all
all: CFLAGS += $(CRELEASE)
all: LDFLAGS += $(LDRELEASE)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

include /opt/cs2110-tools/GBAVariables.mak

debug : CFLAGS += $(CDEBUG)
debug : LDFLAGS += $(LDDEBUG)
debug : $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba : $(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf : crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OFILES) libc_sbrk.o
	@$(CC) $(CFLAGS) -o $(PROGNAME).elf $^ $(LDFLAGS)

%.o : %.c
	@echo "[COMPILE] Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s                                                                        
	@echo "[ASSEMBLE] Assembling $<"                                               
	@$(AS) $(MODEL) $< -o $@

.PHONY : vba
vba : CFLAGS += $(CRELEASE)
vba : LDFLAGS += $(LDRELEASE)
vba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@vbam $(VBAOPT) $(PROGNAME).gba

.PHONY : mgba
mgba : CFLAGS += $(CRELEASE)
mgba : LDFLAGS += $(LDRELEASE)
mgba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@mgba $(PROGNAME).gba

.PHONY : med
med : CFLAGS += $(CRELEASE)
med : LDFLAGS += $(LDRELEASE)
med : $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@mednafen $(PROGNAME).gba

.PHONY : clean
clean :
	@echo "[CLEAN] Removing all compiled files"
	@rm -f *.o *.elf *.gba images/*.o text/*.o
