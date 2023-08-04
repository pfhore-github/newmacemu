cmake_minimum_required(VERSION 3.26)
project(newmacemu)
enable_testing()
include(CTest)
include(CheckIPOSupported)


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
add_executable(newmacemu ${SRC_CPU} 
src/main.cc src/debugger/debugger.cc src/io.cc src/chip/via.cc src/ebus/adb.cc src/video.cc
src/chip/rtc.cc
src/rom/macrom.cc
)
add_executable(testcpu ${SRC_CPU} test/test_main.cc 
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

test/debugger/disasm_ea.cc
test/debugger/disasm0.cc
test/debugger/disasm1_3.cc
test/debugger/disasm4.cc
test/debugger/disasm5_7.cc
test/debugger/disasm8_D.cc
test/debugger/disasmE.cc
test/debugger/disasmF.cc
) 
if(result)
  set_property(TARGET newmacemu PROPERTY INTERPROCEDURAL_OPTIMIZATION_Release TRUE)
  set_property(TARGET testcpu PROPERTY INTERPROCEDURAL_OPTIMIZATION_Release TRUE)
endif()
target_compile_definitions(testcpu PUBLIC CI)
target_link_libraries(newmacemu gmp mpfr SDL2::SDL2 fmt SDL2_net Threads::Threads SDL2_ttf)
target_include_directories(newmacemu PRIVATE "./include")
target_link_libraries(testcpu gmp mpfr SDL2::SDL2 fmt Boost::unit_test_framework)
target_include_directories(testcpu PRIVATE "./include" ${Boost_INCLUDE_DIRS})

add_test(NAME ea COMMAND $<TARGET_FILE:testcpu> -t "EA/*")
add_test(NAME abcd COMMAND $<TARGET_FILE:testcpu> -t "ABCD/*")
add_test(NAME add COMMAND $<TARGET_FILE:testcpu> -t "ADD/*")
add_test(NAME adda COMMAND $<TARGET_FILE:testcpu> -t "ADDA/*")
add_test(NAME addi COMMAND $<TARGET_FILE:testcpu> -t "ADDI/*")
add_test(NAME addq COMMAND $<TARGET_FILE:testcpu> -t "ADDQ/*")
add_test(NAME addx COMMAND $<TARGET_FILE:testcpu> -t "ADDX/*")
add_test(NAME and COMMAND $<TARGET_FILE:testcpu> -t "AND/*")
add_test(NAME andi COMMAND $<TARGET_FILE:testcpu> -t "ANDI/*")
add_test(NAME asl COMMAND $<TARGET_FILE:testcpu> -t "ASL/*")
add_test(NAME asr COMMAND $<TARGET_FILE:testcpu> -t "ASR/*")
add_test(NAME bcc COMMAND $<TARGET_FILE:testcpu> -t "Bcc/*")
add_test(NAME bchg COMMAND $<TARGET_FILE:testcpu> -t "BCHG/*")
add_test(NAME bclr COMMAND $<TARGET_FILE:testcpu> -t "BCLR/*")
add_test(NAME bfchg COMMAND $<TARGET_FILE:testcpu> -t "BFCHG/*")
add_test(NAME bfclr COMMAND $<TARGET_FILE:testcpu> -t "BFCLR/*")
add_test(NAME bfexts COMMAND $<TARGET_FILE:testcpu> -t "BFEXTS/*")
add_test(NAME bfextu COMMAND $<TARGET_FILE:testcpu> -t "BFEXTU/*")
add_test(NAME bfffo COMMAND $<TARGET_FILE:testcpu> -t "BFFFO/*")
add_test(NAME bfins COMMAND $<TARGET_FILE:testcpu> -t "BFINS/*")
add_test(NAME bfset COMMAND $<TARGET_FILE:testcpu> -t "BFSET/*")
add_test(NAME bftst COMMAND $<TARGET_FILE:testcpu> -t "BFTST/*")
add_test(NAME bkpt COMMAND $<TARGET_FILE:testcpu> -t "BKPT/*")
add_test(NAME bra COMMAND $<TARGET_FILE:testcpu> -t "BRA/*")
add_test(NAME bsr COMMAND $<TARGET_FILE:testcpu> -t "BSR/*")
add_test(NAME bset COMMAND $<TARGET_FILE:testcpu> -t "BSET/*")
add_test(NAME cas COMMAND $<TARGET_FILE:testcpu> -t "CAS/*")
add_test(NAME cas2 COMMAND $<TARGET_FILE:testcpu> -t "CAS2/*")
add_test(NAME chk COMMAND $<TARGET_FILE:testcpu> -t "CHK/*")
add_test(NAME chk2 COMMAND $<TARGET_FILE:testcpu> -t "CHK2/*")
add_test(NAME clr COMMAND $<TARGET_FILE:testcpu> -t "CLR/*")
add_test(NAME cmp COMMAND $<TARGET_FILE:testcpu> -t "CMP/*")
add_test(NAME cmpa COMMAND $<TARGET_FILE:testcpu> -t "CMPA/*")
add_test(NAME cmpi COMMAND $<TARGET_FILE:testcpu> -t "CMPI/*")
add_test(NAME cmpm COMMAND $<TARGET_FILE:testcpu> -t "CMPM/*")
add_test(NAME cmp2 COMMAND $<TARGET_FILE:testcpu> -t "CMP2/*")
add_test(NAME dbcc COMMAND $<TARGET_FILE:testcpu> -t "DBcc/*")
add_test(NAME divs COMMAND $<TARGET_FILE:testcpu> -t "DIVS/*")
add_test(NAME divu COMMAND $<TARGET_FILE:testcpu> -t "DIVU/*")
add_test(NAME eor COMMAND $<TARGET_FILE:testcpu> -t "EOR/*")
add_test(NAME eori COMMAND $<TARGET_FILE:testcpu> -t "EORI/*")
add_test(NAME exg COMMAND $<TARGET_FILE:testcpu> -t "EXG/*")
add_test(NAME ext COMMAND $<TARGET_FILE:testcpu> -t "EXT/*")
add_test(NAME illegal COMMAND $<TARGET_FILE:testcpu> -t "ILLEGAL")
add_test(NAME jmp COMMAND $<TARGET_FILE:testcpu> -t "JMP/*")
add_test(NAME jsr COMMAND $<TARGET_FILE:testcpu> -t "JSR/*")
add_test(NAME lea COMMAND $<TARGET_FILE:testcpu> -t "LEA/*")
add_test(NAME link COMMAND $<TARGET_FILE:testcpu> -t "LINK/*")
add_test(NAME lsl COMMAND $<TARGET_FILE:testcpu> -t "LSL/*")
add_test(NAME lsr COMMAND $<TARGET_FILE:testcpu> -t "LSR/*")
add_test(NAME move COMMAND $<TARGET_FILE:testcpu> -t "MOVE/*")
add_test(NAME movea COMMAND $<TARGET_FILE:testcpu> -t "MOVEA/*")
add_test(NAME move16 COMMAND $<TARGET_FILE:testcpu> -t "MOVE16/*")
add_test(NAME movem COMMAND $<TARGET_FILE:testcpu> -t "MOVEM/*")
add_test(NAME movep COMMAND $<TARGET_FILE:testcpu> -t "MOVEP/*")
add_test(NAME moveq COMMAND $<TARGET_FILE:testcpu> -t "MOVEQ/*")
add_test(NAME muls COMMAND $<TARGET_FILE:testcpu> -t "MULS/*")
add_test(NAME mulu COMMAND $<TARGET_FILE:testcpu> -t "MULU/*")
add_test(NAME nbcd COMMAND $<TARGET_FILE:testcpu> -t "NBCD/*")
add_test(NAME neg COMMAND $<TARGET_FILE:testcpu> -t "NEG/*")
add_test(NAME negx COMMAND $<TARGET_FILE:testcpu> -t "NEGX/*")
add_test(NAME nop COMMAND $<TARGET_FILE:testcpu> -t "NOP/*")
add_test(NAME not COMMAND $<TARGET_FILE:testcpu> -t "NOT/*")
add_test(NAME or COMMAND $<TARGET_FILE:testcpu> -t "OR/*")
add_test(NAME ori COMMAND $<TARGET_FILE:testcpu> -t "ORI/*")
add_test(NAME pack COMMAND $<TARGET_FILE:testcpu> -t "PACK/*")
add_test(NAME pea COMMAND $<TARGET_FILE:testcpu> -t "PEA/*")
add_test(NAME rol COMMAND $<TARGET_FILE:testcpu> -t "ROL/*")
add_test(NAME ror COMMAND $<TARGET_FILE:testcpu> -t "ROR/*")
add_test(NAME roxl COMMAND $<TARGET_FILE:testcpu> -t "ROXL/*")
add_test(NAME roxr COMMAND $<TARGET_FILE:testcpu> -t "ROXR/*")
add_test(NAME rtd COMMAND $<TARGET_FILE:testcpu> -t "RTD/*")
add_test(NAME rtr COMMAND $<TARGET_FILE:testcpu> -t "RTR/*")
add_test(NAME rts COMMAND $<TARGET_FILE:testcpu> -t "RTS/*")
add_test(NAME sbcd COMMAND $<TARGET_FILE:testcpu> -t "SBCD/*")
add_test(NAME sub COMMAND $<TARGET_FILE:testcpu> -t "SUB/*")
add_test(NAME suba COMMAND $<TARGET_FILE:testcpu> -t "SUBA/*")
add_test(NAME subi COMMAND $<TARGET_FILE:testcpu> -t "SUBI/*")
add_test(NAME subq COMMAND $<TARGET_FILE:testcpu> -t "SUBQ/*")
add_test(NAME subx COMMAND $<TARGET_FILE:testcpu> -t "SUBX/*")
add_test(NAME swap COMMAND $<TARGET_FILE:testcpu> -t "SWAP/*")
add_test(NAME tas COMMAND $<TARGET_FILE:testcpu> -t "TAS/*")
add_test(NAME trap COMMAND $<TARGET_FILE:testcpu> -t "TRAP/*")
add_test(NAME trapcc COMMAND $<TARGET_FILE:testcpu> -t "TRAPcc/*")
add_test(NAME trapv COMMAND $<TARGET_FILE:testcpu> -t "TRAPV/*")
add_test(NAME tst COMMAND $<TARGET_FILE:testcpu> -t "TST/*")
add_test(NAME unlk COMMAND $<TARGET_FILE:testcpu> -t "UNLK/*")
add_test(NAME unpk COMMAND $<TARGET_FILE:testcpu> -t "UNPK/*")


add_test(NAME fabs COMMAND $<TARGET_FILE:testcpu> -t "FABS/*")
add_test(NAME fsabs COMMAND $<TARGET_FILE:testcpu> -t "FSABS/*")
add_test(NAME fdabs COMMAND $<TARGET_FILE:testcpu> -t "FDABS/*")
add_test(NAME facos COMMAND $<TARGET_FILE:testcpu> -t "FACOS/*")
add_test(NAME fadd COMMAND $<TARGET_FILE:testcpu> -t "FADD/*")
add_test(NAME fsadd COMMAND $<TARGET_FILE:testcpu> -t "FSADD/*")
add_test(NAME fdadd COMMAND $<TARGET_FILE:testcpu> -t "FDADD/*")
add_test(NAME fasin COMMAND $<TARGET_FILE:testcpu> -t "FASIN/*")
add_test(NAME fatan COMMAND $<TARGET_FILE:testcpu> -t "FATAN/*")
add_test(NAME fatanh COMMAND $<TARGET_FILE:testcpu> -t "FATANH/*")
add_test(NAME fbcc COMMAND $<TARGET_FILE:testcpu> -t "FBcc/*")
add_test(NAME fcmp COMMAND $<TARGET_FILE:testcpu> -t "FCMP/*")
add_test(NAME fcos COMMAND $<TARGET_FILE:testcpu> -t "FCOS/*")
add_test(NAME fcosh COMMAND $<TARGET_FILE:testcpu> -t "FCOSH/*")
add_test(NAME fdbcc COMMAND $<TARGET_FILE:testcpu> -t "FDBcc/*")
add_test(NAME fetox COMMAND $<TARGET_FILE:testcpu> -t "FETOX/*")
add_test(NAME fetoxm1 COMMAND $<TARGET_FILE:testcpu> -t "FETOXM1/*")
add_test(NAME fdiv COMMAND $<TARGET_FILE:testcpu> -t "FDIV/*")
add_test(NAME fsdiv COMMAND $<TARGET_FILE:testcpu> -t "FSDIV/*")
add_test(NAME fddiv COMMAND $<TARGET_FILE:testcpu> -t "FDDIV/*")
add_test(NAME fgettexp COMMAND $<TARGET_FILE:testcpu> -t "FGETEXP/*")
add_test(NAME fgetman COMMAND $<TARGET_FILE:testcpu> -t "FGETMAN/*")
add_test(NAME fint COMMAND $<TARGET_FILE:testcpu> -t "FINT/*")
add_test(NAME fintrz COMMAND $<TARGET_FILE:testcpu> -t "FINTRZ/*")
add_test(NAME flog10 COMMAND $<TARGET_FILE:testcpu> -t "FLOG10/*")
add_test(NAME flog2 COMMAND $<TARGET_FILE:testcpu> -t "FLOG2/*")
add_test(NAME flogn COMMAND $<TARGET_FILE:testcpu> -t "FLOGN/*")
add_test(NAME flognp1 COMMAND $<TARGET_FILE:testcpu> -t "FLOGNP1/*")
add_test(NAME fmod COMMAND $<TARGET_FILE:testcpu> -t "FMOD/*")
add_test(NAME fmove COMMAND $<TARGET_FILE:testcpu> -t "FMOVE/*")
add_test(NAME fsmove COMMAND $<TARGET_FILE:testcpu> -t "FSMOVE/*")
add_test(NAME fdmove COMMAND $<TARGET_FILE:testcpu> -t "FDMOVE/*")
add_test(NAME fmovecr COMMAND $<TARGET_FILE:testcpu> -t "FMOVECR/*")
add_test(NAME fmovem COMMAND $<TARGET_FILE:testcpu> -t "FMOVEM/*")
add_test(NAME fmul COMMAND $<TARGET_FILE:testcpu> -t "FMUL/*")
add_test(NAME fsmul COMMAND $<TARGET_FILE:testcpu> -t "FSMUL/*")
add_test(NAME fdmul COMMAND $<TARGET_FILE:testcpu> -t "FDMUL/*")
add_test(NAME fneg COMMAND $<TARGET_FILE:testcpu> -t "FNEG/*")
add_test(NAME fsneg COMMAND $<TARGET_FILE:testcpu> -t "FSNEG/*")
add_test(NAME fdneg COMMAND $<TARGET_FILE:testcpu> -t "FDNEG/*")
add_test(NAME fnop COMMAND $<TARGET_FILE:testcpu> -t "FNOP")
add_test(NAME frem COMMAND $<TARGET_FILE:testcpu> -t "FREM/*")
add_test(NAME fscale COMMAND $<TARGET_FILE:testcpu> -t "FSCALE/*")
add_test(NAME fsgldiv COMMAND $<TARGET_FILE:testcpu> -t "FSGLDIV/*")
add_test(NAME fsglmul COMMAND $<TARGET_FILE:testcpu> -t "FSGLMUL/*")
add_test(NAME fsin COMMAND $<TARGET_FILE:testcpu> -t "FSIN/*")
add_test(NAME fsincos COMMAND $<TARGET_FILE:testcpu> -t "FSINCOS/*")
add_test(NAME fsinh COMMAND $<TARGET_FILE:testcpu> -t "FSINH/*")
add_test(NAME fsqrt COMMAND $<TARGET_FILE:testcpu> -t "FSQRT/*")
add_test(NAME fssqrt COMMAND $<TARGET_FILE:testcpu> -t "FSSQRT/*")
add_test(NAME fdsqrt COMMAND $<TARGET_FILE:testcpu> -t "FDSQRT/*")
add_test(NAME fsub COMMAND $<TARGET_FILE:testcpu> -t "FSUB/*")
add_test(NAME fssub COMMAND $<TARGET_FILE:testcpu> -t "FSSUB/*")
add_test(NAME fdsub COMMAND $<TARGET_FILE:testcpu> -t "FDSUB/*")
add_test(NAME ftan COMMAND $<TARGET_FILE:testcpu> -t "FTAN/*")
add_test(NAME ftanh COMMAND $<TARGET_FILE:testcpu> -t "FTANH/*")
add_test(NAME ftentox COMMAND $<TARGET_FILE:testcpu> -t "FTENTOX/*")
add_test(NAME ftrapcc COMMAND $<TARGET_FILE:testcpu> -t "FTRAPcc/*")
add_test(NAME ftst COMMAND $<TARGET_FILE:testcpu> -t "FTST/*")
add_test(NAME ftwotox COMMAND $<TARGET_FILE:testcpu> -t "FTWOTOX/*")
add_test(NAME fp_exception COMMAND $<TARGET_FILE:testcpu> -t "FP_exception/*")

add_test(NAME ittr COMMAND $<TARGET_FILE:testcpu> -t "ITTR/*")
add_test(NAME dttr COMMAND $<TARGET_FILE:testcpu> -t "DTTR/*")
add_test(NAME mmu_disabled COMMAND $<TARGET_FILE:testcpu> -t "MMU_Disabled")
add_test(NAME atc COMMAND $<TARGET_FILE:testcpu> -t "ATC/*")
add_test(NAME psearch COMMAND $<TARGET_FILE:testcpu> -t "PSERACH/*")

add_test(NAME cinv COMMAND $<TARGET_FILE:testcpu> -t "CINV/*")
add_test(NAME cpush COMMAND $<TARGET_FILE:testcpu> -t "CPUSH/*")
add_test(NAME frestore COMMAND $<TARGET_FILE:testcpu> -t "FRESTORE/*")
add_test(NAME fsave COMMAND $<TARGET_FILE:testcpu> -t "FSAVE/*")
add_test(NAME movec COMMAND $<TARGET_FILE:testcpu> -t "MOVEC/*")
add_test(NAME moves COMMAND $<TARGET_FILE:testcpu> -t "MOVES/*")
add_test(NAME pflush COMMAND $<TARGET_FILE:testcpu> -t "PFLUSH*/*")
add_test(NAME ptest COMMAND $<TARGET_FILE:testcpu> -t "PTEST*/*")
add_test(NAME reset COMMAND $<TARGET_FILE:testcpu> -t "RESET/*")
add_test(NAME rte COMMAND $<TARGET_FILE:testcpu> -t "RTE/*")
add_test(NAME stop COMMAND $<TARGET_FILE:testcpu> -t "STOP/*")

add_test(NAME disasm COMMAND $<TARGET_FILE:testcpu> -t "Disasm/*")


