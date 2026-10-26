cmake_minimum_required(VERSION 3.15)
project(OneLineSolver CXX)

# Bật C++17 vì code của bạn dùng cấu trúc auto [r, c]
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tự động tải thư viện Crow
include(FetchContent)
FetchContent_Declare(
  Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG v1.2.0 # Phiên bản ổn định mới nhất
)
FetchContent_MakeAvailable(Crow)

# Tạo file thực thi từ main.cpp
add_executable(solver main.cpp)

# Link thư viện đa luồng (Crow bắt buộc)
find_package(Threads REQUIRED)
target_link_libraries(solver PRIVATE Crow::Crow Threads::Threads)

# Nếu bạn dùng Windows (MSYS2/MinGW), cần link thêm thư viện mạng của Windows
if(WIN32)
    target_link_libraries(solver PRIVATE ws2_32 wsock32)
endif()