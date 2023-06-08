project(newmacemu)
enable_testing()
cmake_minimum_required(VERSION 3.25)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
find_package(Boost REQUIRED CONFIG REQUIRED COMPONENTS unit_test_framework)
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
add_executable(newmacemu src/cpu/m68k_ea.cc src/cpu/68040_mmu.cc src/cpu/exception.cc src/cpu/op_movem.cc src/cpu/68040_fpu.cc src/cpu/opcodes.cc src/68040.cc src/bus.cc src/memory.cc src/io.cc)
add_executable(testcpu test/test_main.cc 
test/cpu/ea.cc 
test/cpu/abcd.cc test/cpu/nbcd.cc
test/cpu/add.cc test/cpu/adda.cc test/cpu/addi.cc test/cpu/addq.cc test/cpu/addx.cc 
test/cpu/and.cc test/cpu/andi.cc
test/cpu/cmp.cc test/cpu/cmpa.cc test/cpu/cmpi.cc test/cpu/cmpm.cc test/cpu/cmp2.cc
test/cpu/asl.cc test/cpu/asr.cc test/cpu/lsl.cc  test/cpu/lsr.cc
test/cpu/bcc.cc test/cpu/bra.cc test/cpu/dbcc.cc test/cpu/jmp.cc test/cpu/jsr.cc test/cpu/lea.cc
test/cpu/bchg.cc test/cpu/bclr.cc test/cpu/bset.cc
test/cpu/bfchg.cc test/cpu/bfclr.cc test/cpu/bfexts.cc test/cpu/bfextu.cc test/cpu/bfffo.cc test/cpu/bfins.cc test/cpu/bfset.cc test/cpu/bftst.cc
test/cpu/bkpt.cc test/cpu/chk.cc  test/cpu/chk2.cc
test/cpu/cas.cc test/cpu/cas2.cc
test/cpu/clr.cc test/cpu/neg.cc
test/cpu/divs.cc test/cpu/divu.cc test/cpu/muls.cc  test/cpu/mulu.cc
test/cpu/eor.cc test/cpu/eori.cc
test/cpu/exg.cc
test/cpu/ext.cc
test/cpu/ori.cc
test/cpu/illegal.cc 
test/cpu/link.cc
test/cpu/move.cc test/cpu/movea.cc test/cpu/move16.cc test/cpu/movem.cc test/cpu/movep.cc test/cpu/moveq.cc
test/arithmetric.cc test/nz.cc test/store.cc test/bits.cc test/fpu.cc src/cpu/m68k_ea.cc src/cpu/68040_fpu.cc src/cpu/68040_mmu.cc src/cpu/exception.cc src/cpu/op_movem.cc src/cpu/opcodes.cc src/bus.cc src/memory.cc src/io.cc)
target_link_libraries(newmacemu gmp mpfr SDL2::SDL2 fmt)
target_include_directories(newmacemu PRIVATE "./include")
target_link_libraries(testcpu gmp mpfr SDL2::SDL2 fmt Boost::unit_test_framework)
target_include_directories(testcpu PRIVATE "./include" ${Boost_INCLUDE_DIRS})

add_test(NAME core COMMAND $<TARGET_FILE:testcpu> )
add_test(NAME ea/r COMMAND $<TARGET_FILE:testcpu> --run_test=EA/Read/*)
add_test(NAME ea/w COMMAND $<TARGET_FILE:testcpu> --run_test=EA/Write/*)
add_test(NAME ea/addr COMMAND $<TARGET_FILE:testcpu> --run_test=EA/ADDR/*)
add_test(NAME abcd COMMAND $<TARGET_FILE:testcpu> --run_test=ABCD/*)
add_test(NAME add COMMAND $<TARGET_FILE:testcpu> --run_test=ADD/*)
add_test(NAME adda COMMAND $<TARGET_FILE:testcpu> --run_test=ADDA/*)
add_test(NAME addi COMMAND $<TARGET_FILE:testcpu> --run_test=ADDI/*)
add_test(NAME addq COMMAND $<TARGET_FILE:testcpu> --run_test=ADDQ/*)
add_test(NAME addx COMMAND $<TARGET_FILE:testcpu> --run_test=ADDX/*)
add_test(NAME and COMMAND $<TARGET_FILE:testcpu> --run_test=AND/*)
add_test(NAME andi COMMAND $<TARGET_FILE:testcpu> --run_test=ANDI/*)
add_test(NAME asl COMMAND $<TARGET_FILE:testcpu> --run_test=ASL/*)
add_test(NAME asr COMMAND $<TARGET_FILE:testcpu> --run_test=ASR/*)
add_test(NAME bcc COMMAND $<TARGET_FILE:testcpu> --run_test=Bcc/*)
add_test(NAME bchg COMMAND $<TARGET_FILE:testcpu> --run_test=BCHG/*)
add_test(NAME bclr COMMAND $<TARGET_FILE:testcpu> --run_test=BCLR/*)
add_test(NAME bfchg COMMAND $<TARGET_FILE:testcpu> --run_test=BFCHG/*)
add_test(NAME bfclr COMMAND $<TARGET_FILE:testcpu> --run_test=BFCLR/*)
add_test(NAME bfexts COMMAND $<TARGET_FILE:testcpu> --run_test=BFEXTS/*)
add_test(NAME bfextu COMMAND $<TARGET_FILE:testcpu> --run_test=BFEXTU/*)
add_test(NAME bfffo COMMAND $<TARGET_FILE:testcpu> --run_test=BFFFO/*)
add_test(NAME bfins COMMAND $<TARGET_FILE:testcpu> --run_test=BFINS/*)
add_test(NAME bfset COMMAND $<TARGET_FILE:testcpu> --run_test=BFSET/*)
add_test(NAME bftst COMMAND $<TARGET_FILE:testcpu> --run_test=BFTST/*)
add_test(NAME bkpt COMMAND $<TARGET_FILE:testcpu> --run_test=BKPT/*)
add_test(NAME bra COMMAND $<TARGET_FILE:testcpu> --run_test=BRA/*)
add_test(NAME bset COMMAND $<TARGET_FILE:testcpu> --run_test=BSET/*)
add_test(NAME cas COMMAND $<TARGET_FILE:testcpu> --run_test=CAS/*)
add_test(NAME cas2 COMMAND $<TARGET_FILE:testcpu> --run_test=CAS2/*)
add_test(NAME chk COMMAND $<TARGET_FILE:testcpu> --run_test=CHK/*)
add_test(NAME chk2 COMMAND $<TARGET_FILE:testcpu> --run_test=CHK2/*)
add_test(NAME clr COMMAND $<TARGET_FILE:testcpu> --run_test=CLR/*)
add_test(NAME cmp COMMAND $<TARGET_FILE:testcpu> --run_test=CMP/*)
add_test(NAME cmpa COMMAND $<TARGET_FILE:testcpu> --run_test=CMPA/*)
add_test(NAME cmpi COMMAND $<TARGET_FILE:testcpu> --run_test=CMPI/*)
add_test(NAME cmpm COMMAND $<TARGET_FILE:testcpu> --run_test=CMPM/*)
add_test(NAME cmp2 COMMAND $<TARGET_FILE:testcpu> --run_test=CMP2/*)
add_test(NAME dbcc COMMAND $<TARGET_FILE:testcpu> --run_test=DBcc/*)
add_test(NAME divs COMMAND $<TARGET_FILE:testcpu> --run_test=DIVS/*)
add_test(NAME divu COMMAND $<TARGET_FILE:testcpu> --run_test=DIVU/*)
add_test(NAME eor COMMAND $<TARGET_FILE:testcpu> --run_test=EOR/*)
add_test(NAME eori COMMAND $<TARGET_FILE:testcpu> --run_test=EORI/*)
add_test(NAME exg COMMAND $<TARGET_FILE:testcpu> --run_test=EXG/*)
add_test(NAME ext COMMAND $<TARGET_FILE:testcpu> --run_test=EXT/*)
add_test(NAME illegal COMMAND $<TARGET_FILE:testcpu> --run_test=ILLEGAL)
add_test(NAME jmp COMMAND $<TARGET_FILE:testcpu> --run_test=JMP)
add_test(NAME jsr COMMAND $<TARGET_FILE:testcpu> --run_test=JSR)
add_test(NAME lea COMMAND $<TARGET_FILE:testcpu> --run_test=LEA)
add_test(NAME link COMMAND $<TARGET_FILE:testcpu> --run_test=LINK/*)
add_test(NAME lsl COMMAND $<TARGET_FILE:testcpu> --run_test=LSL/*)
add_test(NAME lsr COMMAND $<TARGET_FILE:testcpu> --run_test=LSR/*)
add_test(NAME move COMMAND $<TARGET_FILE:testcpu> --run_test=MOVE/*)
add_test(NAME movea COMMAND $<TARGET_FILE:testcpu> --run_test=MOVEA/*)
add_test(NAME move16 COMMAND $<TARGET_FILE:testcpu> --run_test=MOVE16/*)
add_test(NAME movem COMMAND $<TARGET_FILE:testcpu> --run_test=MOVEM/*)
add_test(NAME movep COMMAND $<TARGET_FILE:testcpu> --run_test=MOVEP/*)
add_test(NAME moveq COMMAND $<TARGET_FILE:testcpu> --run_test=MOVEQ/*)
add_test(NAME muls COMMAND $<TARGET_FILE:testcpu> --run_test=MULS/*)
add_test(NAME mulu COMMAND $<TARGET_FILE:testcpu> --run_test=MULU/*)
add_test(NAME nbcd COMMAND $<TARGET_FILE:testcpu> --run_test=NBCD/*)
add_test(NAME neg COMMAND $<TARGET_FILE:testcpu> --run_test=NEG/*)
add_test(NAME ori COMMAND $<TARGET_FILE:testcpu> --run_test=ORI/*)
