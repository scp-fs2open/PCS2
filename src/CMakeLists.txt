
set (PCS2_SOURCES
	AsyncProgress.cpp
	BSPDataStructs.cpp
	BSPHandler.cpp
	COBHandler.cpp
	color_options_dlg.cpp
	combo.cpp
	DAEHandler.cpp
	editor.cpp
	FileList.cpp
	geo_sphere.cpp
	GLee.c
	Main_Panel.cpp
	matrix3d.cpp
	matrix.cpp
	MOI.cpp
	ogl_vertex_buffers.cpp
	op_stack.cpp
	pcs2.cpp
	pcs2_filethread.cpp
	pcs_file.cpp
	pcs_file_dstructs.cpp
	pcs_pmf_cob.cpp
	pcs_pmf_dae.cpp
	pcs_pmf_pof.cpp
	pcs_pof_bspfuncs.cpp
	POFHandler.cpp
	tex_ctrl.cpp
	text_ctrl.cpp
	vector3d.cpp
	versions.cpp
	VPReader.cpp
	wxGL_PMFCanvas.cpp
	wxPCSPref.cpp
	GLee.c
)

set (PCS2_HEADERS
	ACEN_editor.h
	array_ctrl.h
	AsyncProgress.h
	BSPDataStructs.h
	BSPHandler.h
	chunk_editors.h
	COBDataStructs.h
	COBHandler.h
	color.h
	color_options_dlg.h
	combo.h
	DAEDataStructs.h
	DAEHandler.h
	debug_window.h
	DOCK_editor.h
	editor.h
	EYE_editor.h
	FileList.h
	FUEL_editor.h
	geo_sphere.h
	GLee.h
	GLOW_editor.h
	glow_points.h
	hardpoint_editor.h
	header_editor.h
	INSG_editor.h
	insignia.png.h
	main_panel.h
	matrix3d.h
	matrix.h
	model_editor_ctrl.h
	MOI.h
	ogl_vertex_buffers.h
	omnipoints.h
	op_stack.h
	PATH_editor.h
	pcs2_CIDs.h
	pcs2_filethread.h
	pcs2.h
	pcs_file_dstructs.h
	pcs_file.h
	pcs_pof_bspfuncs.h
	PINF_editor.h
	POFDataStructs.h
	POFHandler.h
	primitive_ctrl.h
	resource.h
	SHLD_editor.h
	SOBJ_editor.h
	SPCL_editor.h
	tex_ctrl.h
	text_ctrl.h
	TURRET_editor.h
	TXTR_editor.h
	vector3d.h
	VPReader.h
	WPNT_editor.h
	wxGL_PMFCanvas.h
	wxPCSPref.h
)

set(COMPAT_FILES
	compat/filelength.cpp
	compat/filelength.h
	compat/sleep.h
	compat/strncmp.cpp
	compat/strncmp.h
)

set(RESOURCES
)

IF(WIN32)
	set(RESOURCES ${RESOURCES}
		pcs2.ico
		pcs2.rc
	)
endif()

add_executable(pcs2 ${EXE_GUI_TYPE} ${PCS2_SOURCES} ${PCS2_HEADERS} ${COMPAT_FILES} ${RESOURCES})
target_include_directories(pcs2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml")
target_include_directories(pcs2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../res")
target_compile_features(pcs2 PRIVATE cxx_auto_type)

target_link_libraries(pcs2 PRIVATE compiler platform)


find_package(OpenGL REQUIRED)

target_link_libraries(pcs2 PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(pcs2 PRIVATE ${OPENGL_INCLUDE_DIR})


if(WIN32)
	set(Boost_USE_STATIC_LIBS       ON)
	set(Boost_USE_MULTITHREADED     ON)

	set(Boost_USE_STATIC_RUNTIME    OFF)
endif()
find_package(Boost REQUIRED COMPONENTS system filesystem regex)

target_link_libraries(pcs2 PRIVATE ${Boost_LIBRARIES})
target_include_directories(pcs2 PRIVATE ${Boost_INCLUDE_DIRS})


find_package(DevIL REQUIRED)

target_link_libraries(pcs2 PRIVATE ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES})
target_include_directories(pcs2 PRIVATE ${IL_INCLUDE_DIR})
target_compile_definitions(pcs2 PRIVATE ILUT_USE_OPENGL)

if(WIN32)
else()
	set(wxWidgets_USE_DEBUG TRUE)
	set(wxWidgets_USE_UNICODE TRUE)
endif()
find_package(wxWidgets REQUIRED COMPONENTS core base gl adv)

target_include_directories(pcs2 PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_link_libraries(pcs2 PRIVATE ${wxWidgets_LIBRARIES})

target_compile_options(pcs2 PRIVATE ${wxWidgets_CXX_FLAGS})
target_compile_definitions(pcs2 PRIVATE $<$<CONFIG:Debug>:${wxWidgets_DEFINITIONS_DEBUG}> ${wxWidgets_DEFINITIONS})


IF(UNIX)
	find_package(SDL REQUIRED)
	target_link_libraries(pcs2 PRIVATE ${SDL_LIBRARY})
	target_include_directories(pcs2 PRIVATE ${SDL_INCLUDE_DIR})
endif()
