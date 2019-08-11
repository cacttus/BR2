# Install script for directory: C:/git/bottle-world/external/SDL2-2.0.9

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/git/bottle-world/external/SDL2-2.0.9/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/SDL2-staticd.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/SDL2d.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/SDL2d.dll")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/SDL2maind.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake"
         "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/CMakeFiles/Export/cmake/SDL2Targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/CMakeFiles/Export/cmake/SDL2Targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/CMakeFiles/Export/cmake/SDL2Targets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "C:/git/bottle-world/external/SDL2-2.0.9/SDL2Config.cmake"
    "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/SDL2ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_assert.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_atomic.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_audio.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_bits.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_blendmode.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_clipboard.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_android.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_iphoneos.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_macosx.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_minimal.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_os2.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_pandora.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_psp.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_windows.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_winrt.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_config_wiz.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_copying.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_cpuinfo.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_egl.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_endian.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_error.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_events.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_filesystem.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_gamecontroller.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_gesture.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_haptic.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_hints.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_joystick.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_keyboard.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_keycode.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_loadso.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_log.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_main.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_messagebox.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_mouse.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_mutex.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_name.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengl.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengl_glext.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengles.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengles2.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengles2_gl2.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengles2_gl2ext.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengles2_gl2platform.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_opengles2_khrplatform.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_pixels.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_platform.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_power.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_quit.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_rect.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_render.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_revision.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_rwops.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_scancode.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_sensor.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_shape.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_stdinc.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_surface.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_system.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_syswm.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_assert.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_common.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_compare.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_crc32.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_font.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_fuzzer.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_harness.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_images.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_log.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_md5.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_memory.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_test_random.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_thread.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_timer.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_touch.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_types.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_version.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_video.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/SDL_vulkan.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/begin_code.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/include/close_code.h"
    "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/include/SDL_config.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/git/bottle-world/external/SDL2-2.0.9/out/build/x64-Debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
