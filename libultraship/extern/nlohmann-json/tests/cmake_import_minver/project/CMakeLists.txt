cmake_minimum_required(VERSION 3.16...3.31.5)

project(DummyImportMinVer CXX)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target nlohmann_json::nlohmann_json)
