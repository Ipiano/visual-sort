function(add_sanitizer sanitizer)
    add_library(fsanitize_${sanitizer} INTERFACE)
    target_compile_options(fsanitize_${sanitizer} INTERFACE "SHELL:-fsanitize=${sanitizer}")
    target_link_options(fsanitize_${sanitizer} INTERFACE "SHELL:-fsanitize=${sanitizer}")

    add_library(fsanitize::${sanitizer} ALIAS fsanitize_${sanitizer})
endfunction()

if(NOT TARGET fsanitize::address)
    add_sanitizer(address)
    add_sanitizer(undefined)
endif()
