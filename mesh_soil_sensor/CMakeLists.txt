cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mesh_soil_sensor)

target_sources(app PRIVATE
	src/main.c
	src/model_handler.c
	src/soil_srv.c)
target_include_directories(app PRIVATE include)
