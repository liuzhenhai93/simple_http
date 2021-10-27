include(FetchContent)
include(ExternalProject)

set(MONGOOSE_DOWNLOAD_DIR ${THIRD_PARTY_PATH}/download/mongoose)
set(MONGOOSE_INSTALL_DIR ${THIRD_PARTY_PATH}/install/mongoose)


FetchContent_Populate(download_mongoose
        GIT_REPOSITORY https://github.com/cesanta/mongoose.git
        GIT_TAG 6e50fe4475e2f328add167850e1e58b3ef5e512a
        SOURCE_DIR ${MONGOOSE_DOWNLOAD_DIR}
        SUBBUILD_DIR ${THIRD_PARTY_PATH}/tmp
        BINARY_DIR ${THIRD_PARTY_PATH}/tmp
        )

ExternalProject_Add(extern_mongoose
        PREFIX ${THIRD_PARTY_PATH}/mongoose
        SOURCE_DIR ${MONGOOSE_DOWNLOAD_DIR}
        BUILD_IN_SOURCE true
        CONFIGURE_COMMAND ""
        BUILD_COMMAND gcc -c mongoose.c && ar rcs libmongoose.a mongoose.o
        INSTALL_COMMAND mkdir -p ${MONGOOSE_INSTALL_DIR}/{include,lib} && cp mongoose.h  ${MONGOOSE_INSTALL_DIR}/include && cp libmongoose.a ${MONGOOSE_INSTALL_DIR}/lib
        )
add_library(mongoose STATIC IMPORTED GLOBAL)
set_property(TARGET mongoose PROPERTY
        IMPORTED_LOCATION ${MONGOOSE_INSTALL_DIR}/lib/libmongoose.a)
add_dependencies(mongoose extern_mongoose)

include_directories(${MONGOOSE_INSTALL_DIR}/include)