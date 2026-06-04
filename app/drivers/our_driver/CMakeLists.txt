if(CONFIG_OUR_DRIVER)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/our_driver.c)
    target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif ()
