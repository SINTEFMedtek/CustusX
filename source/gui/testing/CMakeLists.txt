if (BUILD_TESTING)

set (cxtestGui_MAIN
    cxtestCatchMain.cpp
    )

set (cxtestGui_MOC_HDRS
    cxtestTestVideoConnectionWidget.h
    )

QT4_WRAP_CPP( cxtestGui_MOCED_HDRS
	${cxtestGui_MOC_HDRS}
	)

set (cxtestGui_SRCS
    ${cxtestGui_MOC_HDRS}
    ${cxtestGui_MOCED_HDRS}
    cxtestCatchStreamingWidgets.cpp
    cxtestTestVideoConnectionWidget.cpp
    )

set (cxtestGui_SRCS
    ${cxtestGui_SRCS}
    PARENT_SCOPE
    )
    
set (cxtestGui_LIBS
    SSC
    cxtestUtilities
    cxGui
)

include_directories(
    .
    ../../testing
    )
    
if (APPLE)
	add_executable(CatchGui MACOSX_BUNDLE ${cxtestGui_MAIN} ${cxtestGui_SRCS} )
else (APPLE)
	if(CX_WINDOWS)
		add_executable(CatchGui WIN32 ${cxtestGui_MAIN} ${cxtestGui_SRCS})
	else(CX_WINDOWS)
		add_executable(CatchGui ${cxtestGui_MAIN} ${cxtestGui_SRCS} )
	endif(CX_WINDOWS)
endif (APPLE)
    
target_link_libraries(CatchGui
    ${cxtestGui_LIBS}
    )
endif (BUILD_TESTING)