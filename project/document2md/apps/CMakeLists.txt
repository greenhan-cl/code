add_executable(doc2md WIN32
    main.cc
    ${PROJECT_SOURCE_DIR}/resources/doc2md.qrc
)

target_link_libraries(doc2md
    PRIVATE
        doc2md::core
)

target_compile_definitions(doc2md
    PRIVATE
        DOC2MD_VERSION="${PROJECT_VERSION}"
)

if(MSVC)
    target_compile_options(doc2md
        PRIVATE
            /utf-8
            /FI${PROJECT_SOURCE_DIR}/include/doc2md/msvc_qt_compat.hpp
    )
endif()

install(TARGETS doc2md
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/3rdparty/markitdown/src/"
    DESTINATION ${CMAKE_INSTALL_BINDIR}/runtime/markitdown/src
    PATTERN "__pycache__" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
)

install(FILES
    "${PROJECT_SOURCE_DIR}/3rdparty/markitdown/ThirdPartyNotices.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/doc2md/licenses/markitdown
)

if(DOC2MD_BUNDLE_PYTHON)
    if(NOT IS_DIRECTORY "${DOC2MD_PYTHON_RUNTIME_DIR}")
        message(FATAL_ERROR
            "DOC2MD_BUNDLE_PYTHON is ON, but DOC2MD_PYTHON_RUNTIME_DIR is invalid"
        )
    endif()

    if(WIN32 AND NOT EXISTS "${DOC2MD_PYTHON_RUNTIME_DIR}/python.exe")
        message(FATAL_ERROR
            "The prepared Windows Python runtime must contain python.exe"
        )
    endif()

    install(DIRECTORY "${DOC2MD_PYTHON_RUNTIME_DIR}/"
        DESTINATION ${CMAKE_INSTALL_BINDIR}/runtime/python
        PATTERN "__pycache__" EXCLUDE
        PATTERN "*.pyc" EXCLUDE
    )
endif()

if(WIN32 AND COMMAND qt_generate_deploy_app_script)
    qt_generate_deploy_app_script(
        TARGET doc2md
        OUTPUT_SCRIPT DOC2MD_QT_DEPLOY_SCRIPT
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${DOC2MD_QT_DEPLOY_SCRIPT})
endif()
