if(BUILD_TESTING)

    #project(OpenGLTestApplication)

    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)

    find_package(VTK REQUIRED)
    include(${VTK_USE_FILE})

    set(SOURCE_FILES
        texture.h
        texture.cpp
        vtkfixture.h
        vtkfixture.cpp
        shadercallback.h
        shadercallback.cpp
    )

    add_executable(OpenGLTestApplication OpenGLTestApplication.cxx ${SOURCE_FILES})
    target_include_directories(OpenGLTestApplication
        PUBLIC
        .
        PRIVATE
        ${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${VTK_INCLUDE_DIRS}
    )
    target_link_libraries(OpenGLTestApplication
        PRIVATE
        cxResource
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${GLEW_LIBRARY}
        ${SSC_GCOV_LIBRARY}
    )

endif(BUILD_TESTING)

