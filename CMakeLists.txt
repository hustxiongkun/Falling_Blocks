#1.cmake version, 指定cmake版本
cmake_minimum_required(VERSION 3.2)

#2.project name, 指定项目的名称，一般和项目的文件夹名称对应
PROJECT(Game_Framework)

#3.set environment variable, 设置环境变量，编译用到的源文件全部都要放到这里，否则编译能通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxx, undefined symbol"
SET(INC_DIR ./third_party/include)
SET(LINK_DIR ./third_party/libs)
#SET(LIBS SDL2 SDL2main SDL2_ttf SDL2_image)

#4.head file path, 头文件目录
INCLUDE_DIRECTORIES(${INC_DIR})

#5.添加库目录
LINK_DIRECTORIES(${LINK_DIR})

#6.要链接的库
#LINK_LIBRARIES(${LIBS})

#7.source directory, 源文件目录
AUX_SOURCE_DIRECTORY(./src DIR_SRCS)

#8.add executable file, 添加要编译的可执行文件
ADD_EXECUTABLE(${PROJECT_NAME} ${DIR_SRCS})
SET(EXECUTABLE_OUTPUT_PATH ./bin)

#9.add link library, 添加可执行文件所需要的库（命名规则：lib+name+.so）
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})