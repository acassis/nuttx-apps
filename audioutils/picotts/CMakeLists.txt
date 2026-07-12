############################################################################
# apps/audioutils/picotts/CMakeLists.txt
############################################################################

if(CONFIG_AUDIOUTILS_PICOTTS)
  set(PICO_DIR ${CMAKE_CURRENT_LIST_DIR}/pico)

  nuttx_add_application(
    NAME        picotts
    SRCS        picotts.c
    SRCS        picorsrc.c
    STACKSIZE   ${CONFIG_PICOTTS_STACK_SIZE}
    PRIORITY    ${CONFIG_PICOTTS_PRIORITY}
    INCDIRS     ${CMAKE_CURRENT_LIST_DIR}/include
    INCDIRS     ${PICO_DIR}
  )

  if(CONFIG_PICOTTS_NSH_CMD)
    nuttx_add_application(
      NAME        picotts_cmd
      SRCS        picotts_cmd.c
      STACKSIZE   4096
      PRIORITY    100
      INCDIRS     ${CMAKE_CURRENT_LIST_DIR}/include
    )
  endif()

  # Pico TTS core library
  set(PICO_SRCS
    ${PICO_DIR}/picoacph.c
    ${PICO_DIR}/picobase.c
    ${PICO_DIR}/picocep.c
    ${PICO_DIR}/picoctrl.c
    ${PICO_DIR}/picodata.c
    ${PICO_DIR}/picodbg.c
    ${PICO_DIR}/picofftsg.c
    ${PICO_DIR}/picokfst.c
    ${PICO_DIR}/picoknow.c
    ${PICO_DIR}/picoktab.c
    ${PICO_DIR}/picoos.c
    ${PICO_DIR}/picopal.c
    ${PICO_DIR}/picopam.c
    ${PICO_DIR}/picopltf.c
    ${PICO_DIR}/picopr.c
    ${PICO_DIR}/picorsrc.c
    ${PICO_DIR}/picosa.c
    ${PICO_DIR}/picosig.c
    ${PICO_DIR}/picosig2.c
    ${PICO_DIR}/picotok.c
    ${PICO_DIR}/picotrns.c
    ${PICO_DIR}/picowa.c
  )

  target_sources(apps PRIVATE ${PICO_SRCS})
endif()
