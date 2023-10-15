cmake_minimum_required(VERSION 3.26)
project(newmacemu)
enable_testing()
include(CTest)
include(CheckIPOSupported)

include(ExternalProject)

check_ipo_supported(RESULT result)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2 )
find_package(SDL2_net REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Threads REQUIRED)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 23) 
# set(CMAKE_CXX_STANDARD_REQUIRED ON) 
find_package(Boost REQUIRED CONFIG REQUIRED COMPONENTS unit_test_framework)
if(CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#  set(CMAKE_EXE_LINKER_FLAGS "--coverage")
endif()
set(SRC_CPU src/cpu/op.cc src/cpu/run.cc src/cpu/m68k_ea.cc src/cpu/bcd.cc src/cpu/arithmetic.cc src/cpu/bits.cc src/cpu/bitfield.cc
src/cpu/68040_mmu.cc src/cpu/branch.cc src/debugger/disasm.cc
src/cpu/exception.cc src/cpu/op_move.cc src/cpu/68040_fpu.cc src/bus.cc src/memory.cc 
)
set(SRC_CHIPS
src/chip/via.cc
src/chip/rtc.cc
src/chip/asc.cc
src/chip/iop.cc
src/chip/mcu.cc
src/chip/scc.cc
src/io.cc
src/video.cc
src/ebus/adb.cc
)
add_executable(newmacemu ${SRC_CPU} 
src/main.cc src/debugger/debugger.cc  ${SRC_CHIPS} src/video.cc
)

add_executable(testrom ${SRC_CPU} ${SRC_CHIPS} test/test_main.cc 
src/io.cc
src/debugger/debugger.cc 
test/rom/common.cc
test/rom/boot.cc
test/rom/init1.cc
test/rom/asc1.cc
test/rom/asc2.cc
test/rom/init2.cc
test/rom/xpram.cc
test/rom/adb1.cc
)

add_executable(testchip ${SRC_CPU} ${SRC_CHIPS} test/test_main.cc 
src/io.cc
test/chip/test_via.cc
)

add_executable(dump_sound rom/util/dump_sound.cc
)

add_executable(testcpu ${SRC_CPU} ${SRC_CHIPS} test/test_main.cc 
test/cpu/ea.cc 
test/cpu/abcd.cc test/cpu/nbcd.cc 
test/cpu/pack.cc test/cpu/sbcd.cc test/cpu/unpk.cc
test/cpu/add.cc test/cpu/adda.cc test/cpu/addi.cc test/cpu/addq.cc test/cpu/addx.cc 
test/cpu/sub.cc test/cpu/suba.cc test/cpu/subi.cc  test/cpu/subq.cc  test/cpu/subx.cc 
test/cpu/and.cc test/cpu/andi.cc
test/cpu/cmp.cc test/cpu/cmpa.cc test/cpu/cmpi.cc test/cpu/cmpm.cc test/cpu/cmp2.cc
test/cpu/asl.cc test/cpu/asr.cc test/cpu/lsl.cc  test/cpu/lsr.cc 
test/cpu/rol.cc test/cpu/ror.cc test/cpu/roxl.cc  test/cpu/roxr.cc
test/cpu/bcc.cc test/cpu/bra.cc test/cpu/bsr.cc test/cpu/dbcc.cc test/cpu/jmp.cc test/cpu/jsr.cc test/cpu/lea.cc test/cpu/pea.cc test/cpu/scc.cc
test/cpu/bchg.cc test/cpu/bclr.cc test/cpu/bset.cc test/cpu/btst.cc
test/cpu/bfchg.cc test/cpu/bfclr.cc test/cpu/bfexts.cc test/cpu/bfextu.cc test/cpu/bfffo.cc test/cpu/bfins.cc test/cpu/bfset.cc test/cpu/bftst.cc
test/cpu/bkpt.cc test/cpu/chk.cc  test/cpu/chk2.cc
test/cpu/cas.cc test/cpu/cas2.cc
test/cpu/clr.cc test/cpu/neg.cc test/cpu/negx.cc test/cpu/not.cc
test/cpu/divs.cc test/cpu/divu.cc test/cpu/muls.cc  test/cpu/mulu.cc
test/cpu/eor.cc test/cpu/eori.cc
test/cpu/exg.cc test/cpu/ext.cc test/cpu/swap.cc
test/cpu/illegal.cc  test/cpu/nop.cc test/cpu/trap.cc  test/cpu/trapcc.cc test/cpu/trapv.cc
test/cpu/link.cc test/cpu/rtd.cc  test/cpu/rtr.cc test/cpu/rts.cc test/cpu/unlk.cc
test/cpu/move.cc test/cpu/movea.cc test/cpu/move16.cc test/cpu/movem.cc test/cpu/movep.cc test/cpu/moveq.cc
test/cpu/or.cc test/cpu/ori.cc
test/cpu/tas.cc test/cpu/tst.cc test/cpu/syscall.cc
test/fpu/common.cc
test/fpu/fabs.cc test/fpu/fneg.cc  test/fpu/fsub.cc test/fpu/ftan.cc  test/fpu/ftanh.cc
test/fpu/facos.cc test/fpu/fasin.cc  test/fpu/fatanh.cc  test/fpu/fsinh.cc  test/fpu/fcosh.cc
test/fpu/fcos.cc test/fpu/fsin.cc test/fpu/fsincos.cc test/fpu/fatan.cc 
test/fpu/fetox.cc test/fpu/fetoxm1.cc test/fpu/fgetexp.cc test/fpu/fgetman.cc test/fpu/fscale.cc test/fpu/ftentox.cc 
test/fpu/flog10.cc test/fpu/flog2.cc  test/fpu/flogn.cc test/fpu/flognp1.cc
test/fpu/fbcc.cc test/fpu/fnop.cc test/fpu/fscc.cc test/fpu/fcmp.cc test/fpu/fdbcc.cc
test/fpu/fadd.cc test/fpu/fdiv.cc test/fpu/fmod.cc test/fpu/fmul.cc test/fpu/frem.cc
test/fpu/fsgldiv.cc test/fpu/fsglmul.cc
test/fpu/fmove.cc test/fpu/fmovecr.cc test/fpu/fmovem.cc test/fpu/ftrapcc.cc test/fpu/ftwotox.cc
test/fpu/fint.cc test/fpu/fintrz.cc test/fpu/fsqrt.cc test/fpu/ftst.cc
test/fpu/fpexcept.cc

test/mmu/ittr.cc test/mmu/dttr.cc test/mmu/disabled.cc test/mmu/atc.cc test/mmu/psearch.cc
test/cpu/cinv.cc test/cpu/cpush.cc test/fpu/frestore.cc test/fpu/fsave.cc
test/cpu/movec.cc test/cpu/moves.cc test/mmu/pflush.cc 
test/cpu/reset.cc test/cpu/rte.cc test/cpu/stop.cc
test/mmu/ptest.cc

) 
if(result)
  set_property(TARGET newmacemu PROPERTY INTERPROCEDURAL_OPTIMIZATION_Release TRUE)
  set_property(TARGET testcpu PROPERTY INTERPROCEDURAL_OPTIMIZATION_Release TRUE)
endif()
target_compile_definitions(testcpu PUBLIC CI)
target_link_libraries(newmacemu gmp mpfr SDL2::SDL2 fmt SDL2_net Threads::Threads SDL2_ttf)
target_include_directories(newmacemu PRIVATE "./include")
target_link_libraries(testcpu gmp mpfr SDL2::SDL2 fmt Boost::unit_test_framework)
target_include_directories(testcpu PRIVATE "./include" "./test" ${Boost_INCLUDE_DIRS})
target_include_directories(testrom PRIVATE "./include" "./test" ${Boost_INCLUDE_DIRS} )
target_include_directories(testchip PRIVATE "./include" "./test" ${Boost_INCLUDE_DIRS})
target_link_libraries(testrom gmp mpfr SDL2::SDL2 fmt  Threads::Threads Boost::unit_test_framework SDL2_ttf)
target_link_libraries(testchip gmp mpfr SDL2::SDL2 fmt  Threads::Threads Boost::unit_test_framework)

target_include_directories(dump_sound PRIVATE "./include")
target_link_libraries(dump_sound  SDL2::SDL2)


add_test(NAME CPU COMMAND $<TARGET_FILE:testcpu> )


add_test(NAME asc COMMAND $<TARGET_FILE:testrom> -t "ASC/*")

add_test(NAME rom/boot COMMAND $<TARGET_FILE:testrom> -t "boot/*")
add_test(NAME rom/asc COMMAND $<TARGET_FILE:testrom> -t "test_asc/*")
add_test(NAME rom/asc2 COMMAND $<TARGET_FILE:testrom> -t "e_asc/*")
add_test(NAME rom/adb1 COMMAND $<TARGET_FILE:testrom> -t "adb1/*")


ExternalProject_Add(
  build_rom                 # Name for custom target
  SOURCE_DIR ../rom
  BINARY_DIR ../rom
  CONFIGURE_COMMAND ""         # Skip configuring the project, e.g. with autoconf
  BUILD_COMMAND
    make 
  INSTALL_COMMAND ""      # This particular build system has no install command
  BUILD_BYPRODUCTS obj/rom.bin
)