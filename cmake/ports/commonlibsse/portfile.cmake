vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO SkyHorizon3/CommonLibSSE
    REF e3764573bbc03d3f6c50459b633921094f36f0d7
    SHA512 508f25a527e6bd0042e19793b60c3f6b1b81ab15f33295e653798fdb91b69b445eb704cb465c959d49de1827f2079eb17d74d607546c563102073ae0040b4400
    HEAD_REF dev
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DSKSE_SUPPORT_XBYAK=ON
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/CommonLibSSE")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

if(EXISTS "${SOURCE_PATH}/LICENSE")
    vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
endif()
