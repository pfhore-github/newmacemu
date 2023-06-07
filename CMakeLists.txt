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
add_executable(newmacemu src/cpu/m68k_ea.cc src/cpu/68040_mmu.cc src/cpu/exception.cc src/cpu/movem.cc src/cpu/68040_fpu.cc src/cpu/opcodes.cc src/68040.cc src/bus.cc src/memory.cc src/io.cc)
add_executable(testcpu test/test_main.cc 
test/cpu/ea.cc 
test/cpu/abcd.cc 
test/cpu/add.cc test/cpu/adda.cc test/cpu/addi.cc test/cpu/addq.cc test/cpu/addx.cc 
test/cpu/and.cc test/cpu/andi.cc
test/cpu/cmp.cc test/cpu/cmpa.cc test/cpu/cmpi.cc test/cpu/cmpm.cc test/cpu/cmp2.cc
test/cpu/asl.cc test/cpu/asr.cc
test/cpu/bcc.cc test/cpu/bra.cc test/cpu/dbcc.cc
test/cpu/bchg.cc test/cpu/bclr.cc test/cpu/bset.cc
test/cpu/bfchg.cc test/cpu/bfclr.cc test/cpu/bfexts.cc test/cpu/bfextu.cc test/cpu/bfffo.cc test/cpu/bfins.cc test/cpu/bfset.cc test/cpu/bftst.cc
test/cpu/bkpt.cc test/cpu/chk.cc  test/cpu/chk2.cc
test/cpu/cas.cc test/cpu/cas2.cc
test/cpu/clr.cc
test/cpu/ori.cc 
test/arithmetric.cc test/nz.cc test/store.cc test/bits.cc test/fpu.cc src/cpu/m68k_ea.cc src/cpu/68040_fpu.cc src/cpu/68040_mmu.cc src/cpu/exception.cc src/cpu/movem.cc src/cpu/opcodes.cc src/bus.cc src/memory.cc src/io.cc)
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
add_test(NAME dbcc COMMAND $<TARGET_FILE:testcpu> --run_test=DBCC/*)
add_test(NAME ori COMMAND $<TARGET_FILE:testcpu> --run_test=ORI/*)
