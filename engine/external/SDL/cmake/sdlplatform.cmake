macro(SDL_DetectCMakePlatform)
  set(SDL_CMAKE_PLATFORM )
  # Get the platform
  if(WIN32)
    set(SDL_CMAKE_PLATFORM Windows)
  elseif(PSP)
    set(SDL_CMAKE_PLATFORM psp)
  elseif(APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES ".*(Darwin|MacOS).*")
      set(SDL_CMAKE_PLATFORM macOS)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*tvOS.*")
      set(SDL_CMAKE_PLATFORM tvOS)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*iOS.*")
      set(SDL_CMAKE_PLATFORM iOS)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*watchOS.*")
      set(SDL_CMAKE_PLATFORM watchOS)
    elseif (CMAKE_SYSTEM_NAME MATCHES "visionOS")
      set(SDL_CMAKE_PLATFORM visionOS)
    else()
      message(WARNING "Unknown Apple platform: \"${CMAKE_SYSTEM_NAME}\"")
    endif()
  elseif(CMAKE_SYSTEM_NAME MATCHES "Haiku.*")
    set(SDL_CMAKE_PLATFORM Haiku)
  elseif(NINTENDO_3DS)
    set(SDL_CMAKE_PLATFORM n3ds)
  elseif(PS2)
    set(SDL_CMAKE_PLATFORM ps2)
  elseif(VITA)
    set(SDL_CMAKE_PLATFORM Vita)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
    set(SDL_CMAKE_PLATFORM Linux)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD.*")
    set(SDL_CMAKE_PLATFORM FreeBSD)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
    set(SDL_CMAKE_PLATFORM NetBSD)
  elseif(CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
    set(SDL_CMAKE_PLATFORM OpenBSD)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*GNU.*")
    set(SDL_CMAKE_PLATFORM GNU)
  elseif(CMAKE_SYSTEM_NAME MATCHES ".*BSDI.*")
    set(SDL_CMAKE_PLATFORM BSDi)
  elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly.*|FreeBSD")
    set(SDL_CMAKE_PLATFORM FreeBSD)
  elseif(CMAKE_SYSTEM_NAME MATCHES "SYSV5.*")
    set(SDL_CMAKE_PLATFORM SYSV5)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Solaris.*|SunOS.*")
    set(SDL_CMAKE_PLATFORM Solaris)
  elseif(CMAKE_SYSTEM_NAME MATCHES "HP-UX.*")
    set(SDL_CMAKE_PLATFORM HPUX)
  elseif(CMAKE_SYSTEM_NAME MATCHES "AIX.*")
    set(SDL_CMAKE_PLATFORM AIX)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Minix.*")
    set(SDL_CMAKE_PLATFORM Minix)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Android.*")
    set(SDL_CMAKE_PLATFORM Android)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Emscripten.*")
    set(SDL_CMAKE_PLATFORM Emscripten)
  elseif(CMAKE_SYSTEM_NAME MATCHES "QNX.*")
    set(SDL_CMAKE_PLATFORM QNX)
  elseif(CMAKE_SYSTEM_NAME MATCHES "BeOS.*")
    message(FATAL_ERROR "BeOS support has been removed as of SDL 2.0.2.")
  endif()

  if(SDL_CMAKE_PLATFORM)
    string(TOUPPER "${SDL_CMAKE_PLATFORM}" _upper_platform)
    set(${_upper_platform} TRUE)
  else()
    set(SDL_CMAKE_PLATFORM "unknown")
  endif()
endmacro()

function(SDL_DetectCPUArchitecture)
  set(sdl_cpu_names)
  if(APPLE AND CMAKE_OSX_ARCHITECTURES)
    foreach(osx_arch ${CMAKE_OSX_ARCHITECTURES})
      if(osx_arch STREQUAL "x86_64")
        list(APPEND sdl_cpu_names "x64")
      elseif(osx_arch STREQUAL "arm64")
        list(APPEND sdl_cpu_names "arm64")
      endif()
    endforeach()
  endif()

  set(sdl_known_archs x64 x86 arm64 arm32 emscripten powerpc64 powerpc32 loongarch64)
  if(NOT sdl_cpu_names)
    set(found FALSE)
    foreach(sdl_known_arch ${sdl_known_archs})
      if(NOT found)
        string(TOUPPER "${sdl_known_arch}" sdl_known_arch_upper)
        set(var_name "SDL_CPU_${sdl_known_arch_upper}")
        check_cpu_architecture(${sdl_known_arch} ${var_name})
        if(${var_name})
          list(APPEND sdl_cpu_names ${sdl_known_arch})
          set(found TRUE)
        endif()
      endif()
    endforeach()
  endif()

  foreach(sdl_known_arch ${sdl_known_archs})
    string(TOUPPER "${sdl_known_arch}" sdl_known_arch_upper)
    set(var_name "SDL_CPU_${sdl_known_arch_upper}")
    if(sdl_cpu_names MATCHES "(^|;)${sdl_known_arch}($|;)")  # FIXME: use if(IN_LIST)
      set(${var_name} 1 PARENT_SCOPE)
    else()
      set(${var_name} 0 PARENT_SCOPE)
    endif()
  endforeach()
  set(SDL_CPU_NAMES ${sdl_cpu_names} PARENT_SCOPE)
endfunction()
